#!/usr/bin/env python3
"""Outils de synchronisation des journaux de bord avec le Google Doc de notes.

La procédure complète est dans INSTRUCTIONS.md (même dossier). Sous-commandes :

  extraire   Lit un export du Google Doc et liste ses blocs (paragraphes,
             images), en signalant ceux qui ne sont pas encore dans un log.
  decoder    Écrit sur disque un fichier Drive reçu en base64 (photo, vidéo).
  compresser Compresse photos et vidéos sous 2 Mo et les range dans medias/.
  ignorer    Marque un bloc comme volontairement laissé hors des journaux.
  hash       Affiche l'identifiant d'un texte ou d'un fichier.
  verifier   Contrôle les logs et les médias avant un commit.

Chaque bloc du document a un identifiant (12 caractères) calculé sur son
contenu. Un log liste dans son en-tête `sources` les identifiants des blocs
dont il est tiré : c'est ce qui permet de ne reprendre que ce qui est nouveau.
"""

import argparse
import base64
import binascii
import hashlib
import html
import io
import json
import re
import shutil
import subprocess
import sys
import unicodedata
import zipfile
from datetime import date
from html.parser import HTMLParser
from pathlib import Path
from urllib.parse import parse_qs, urlparse

DOSSIER_SYNC = Path(__file__).resolve().parent
DOSSIER_JOURNAUX = DOSSIER_SYNC.parent
FICHIER_ETAT = DOSSIER_SYNC / "etat.json"
DOSSIER_TMP = DOSSIER_SYNC / "tmp"

JOURNAUX = ("pami", "robot-principal")
TAILLE_MAX = 2_000_000  # octets : strictement moins de 2 Mo par fichier
COTE_MAX_IMAGE = 1600  # pixels, plus grand côté d'une photo

EXT_IMAGES = {".jpg", ".jpeg", ".png", ".webp", ".gif", ".bmp", ".tif", ".tiff", ".heic"}
EXT_VIDEOS = {".mp4", ".mov", ".m4v", ".webm", ".avi", ".mkv", ".3gp"}

MOIS = {
    "janvier": 1, "fevrier": 2, "mars": 3, "avril": 4, "mai": 5, "juin": 6,
    "juillet": 7, "aout": 8, "septembre": 9, "octobre": 10, "novembre": 11,
    "decembre": 12,
}
RE_DATE_LETTRES = re.compile(
    r"^(?:le\s+)?(?:(?:lundi|mardi|mercredi|jeudi|vendredi|samedi|dimanche)\s+)?"
    r"(\d{1,2})(?:er)?\s+(" + "|".join(MOIS) + r")\s+(\d{4})\s*:?$"
)
RE_DATE_CHIFFRES = re.compile(r"^(?:le\s+)?(\d{1,2})[/.\-](\d{1,2})[/.\-](\d{2}|\d{4})\s*:?$")
RE_NOM_LOG = re.compile(r"^(\d{4}-\d{2}-\d{2})-(\d{2})-[a-z0-9]+(?:-[a-z0-9]+)*\.md$")
RE_HASH = re.compile(r"\b[0-9a-f]{12}\b")
RE_INCLUDE_MEDIA = re.compile(r"{%-?\s*include\s+media\.html\s+[^%]*?fichier=\"([^\"]+)\"")


# ---------------------------------------------------------------------------
# Identifiants et état
# ---------------------------------------------------------------------------

def sans_accents(texte):
    decompose = unicodedata.normalize("NFKD", texte)
    return "".join(c for c in decompose if not unicodedata.combining(c))


def hash_texte(texte):
    """Identifiant d'un paragraphe.

    Seuls les lettres et les chiffres comptent : l'identifiant ne change pas si
    on retouche la ponctuation, les espaces, les accents ou la casse, ni selon
    le format d'export du document (HTML ou texte).
    """
    normalise = "".join(c for c in sans_accents(texte).lower() if c.isalnum())
    return hashlib.sha1(normalise.encode("utf-8")).hexdigest()[:12]


def hash_octets(donnees):
    return hashlib.sha1(donnees).hexdigest()[:12]


def lire_etat():
    if FICHIER_ETAT.exists():
        return json.loads(FICHIER_ETAT.read_text(encoding="utf-8"))
    return {"derniere_synchro": None, "ignores": {}}


def ecrire_etat(etat):
    FICHIER_ETAT.write_text(
        json.dumps(etat, ensure_ascii=False, indent=2) + "\n", encoding="utf-8"
    )


def lire_entete(chemin):
    """En-tête YAML d'un log, lu simplement (clé: valeur sur une ligne)."""
    lignes = chemin.read_text(encoding="utf-8").splitlines()
    if not lignes or lignes[0].strip() != "---":
        return None, lignes
    entete = {}
    for i, ligne in enumerate(lignes[1:], start=1):
        if ligne.strip() == "---":
            return entete, lignes[i + 1:]
        if ":" in ligne and not ligne.startswith((" ", "\t", "#")):
            cle, valeur = ligne.split(":", 1)
            entete[cle.strip()] = valeur.strip().strip('"').strip("'")
    return None, lignes


def fichiers_logs(journal=None):
    for nom in JOURNAUX if journal is None else (journal,):
        yield from sorted((DOSSIER_JOURNAUX / nom / "logs").glob("*.md"))


def sources_connues():
    """Identifiant -> logs qui le citent (ou raison s'il est ignoré)."""
    connues = {}
    for chemin in fichiers_logs():
        entete, _ = lire_entete(chemin)
        if not entete:
            continue
        relatif = chemin.relative_to(DOSSIER_JOURNAUX).as_posix()
        for identifiant in RE_HASH.findall(entete.get("sources", "")):
            connues.setdefault(identifiant, []).append(relatif)
    return connues


# ---------------------------------------------------------------------------
# Lecture d'un export du Google Doc
# ---------------------------------------------------------------------------

def chercher_base64(valeur):
    """Trouve le contenu base64 dans un résultat d'outil enregistré en JSON."""
    if isinstance(valeur, str):
        texte = valeur.strip()
        if texte[:1] in "{[":
            try:
                return chercher_base64(json.loads(texte))
            except ValueError:
                pass
        return texte
    if isinstance(valeur, dict):
        for cle in ("content", "text", "data"):
            if cle in valeur:
                trouve = chercher_base64(valeur[cle])
                if trouve:
                    return trouve
        candidats = [chercher_base64(v) for v in valeur.values()]
    elif isinstance(valeur, list):
        candidats = [chercher_base64(v) for v in valeur]
    else:
        return None
    candidats = [c for c in candidats if c]
    return max(candidats, key=len) if candidats else None


def charger_octets(chemin):
    """Octets d'un fichier, en décodant le base64 d'un résultat d'outil si besoin."""
    brut = chemin.read_bytes()
    if brut[:2] == b"PK" or chemin.suffix.lower() not in (".json", ".txt", ".b64", ""):
        return brut
    try:
        texte = brut.decode("utf-8").strip()
    except UnicodeDecodeError:
        return brut
    charge = chercher_base64(texte) if texte[:1] in "{[" else texte
    if charge and re.fullmatch(r"[A-Za-z0-9+/=_\-\s]+", charge[:4000]):
        try:
            compact = re.sub(r"\s+", "", charge).replace("-", "+").replace("_", "/")
            return base64.b64decode(compact + "=" * (-len(compact) % 4), validate=True)
        except (binascii.Error, ValueError):
            pass
    return brut


def lien_reel(href):
    """Google enveloppe les liens d'un export : on retrouve la vraie adresse."""
    url = urlparse(href)
    if url.netloc.endswith("google.com") and url.path == "/url":
        cible = parse_qs(url.query).get("q")
        if cible:
            return cible[0]
    return href


class LecteurHtml(HTMLParser):
    """Découpe l'export HTML d'un Google Doc en blocs, dans l'ordre du document."""

    BALISES_BLOC = {"p", "h1", "h2", "h3", "h4", "h5", "h6", "li"}

    def __init__(self):
        super().__init__(convert_charrefs=True)
        self.blocs = []
        self.courant = None
        self.dans_style = False

    def handle_starttag(self, balise, attributs):
        attributs = dict(attributs)
        if balise in ("style", "script"):
            self.dans_style = True
        elif balise in self.BALISES_BLOC:
            self.fermer()
            classes = (attributs.get("class") or "").split()
            titre = balise.startswith("h") or "title" in classes or "subtitle" in classes
            self.courant = {"titre": titre, "texte": [], "images": [], "liens": []}
        elif balise == "br" and self.courant:
            self.courant["texte"].append("\n")
        elif balise == "img" and attributs.get("src"):
            self.ouvrir()
            self.courant["images"].append(attributs["src"])
        elif balise == "a" and attributs.get("href") and self.courant:
            lien = lien_reel(attributs["href"])
            if lien.startswith("http"):
                self.courant["liens"].append(lien)

    def handle_endtag(self, balise):
        if balise in ("style", "script"):
            self.dans_style = False
        elif balise in self.BALISES_BLOC:
            self.fermer()

    def handle_data(self, donnees):
        if self.dans_style:
            return
        if donnees.strip():
            self.ouvrir()
        if self.courant:
            self.courant["texte"].append(donnees)

    def ouvrir(self):
        if self.courant is None:
            self.courant = {"titre": False, "texte": [], "images": [], "liens": []}

    def fermer(self):
        if self.courant:
            texte = re.sub(r"[ \t\xa0]+", " ", "".join(self.courant["texte"])).strip()
            if texte or self.courant["images"]:
                self.courant["texte"] = texte
                self.blocs.append(self.courant)
        self.courant = None


def blocs_depuis_texte(texte):
    """Découpe un export texte (ou Markdown) en paragraphes."""
    blocs = []
    for paragraphe in re.split(r"\n\s*\n", texte):
        nettoye = re.sub(r"\\([<>~*_#\[\]().!|-])", r"\1", paragraphe)
        nettoye = re.sub(r"[ \t\xa0]+", " ", nettoye).strip()
        if not nettoye:
            continue
        titre = nettoye.startswith("#") or (len(nettoye) <= 80 and nettoye.endswith(":"))
        liens = re.findall(r"https?://[^\s)>\]]+", nettoye)
        blocs.append({"titre": titre, "texte": nettoye.lstrip("# "), "images": [], "liens": liens})
    return blocs


def lire_date(texte):
    """Date d'une ligne qui ne contient qu'une date, sinon None."""
    ligne = sans_accents(texte).lower().strip()
    trouve = RE_DATE_LETTRES.match(ligne)
    try:
        if trouve:
            return date(int(trouve[3]), MOIS[trouve[2]], int(trouve[1])).isoformat()
        trouve = RE_DATE_CHIFFRES.match(ligne)
        if trouve:
            annee = int(trouve[3])
            return date(annee + 2000 if annee < 100 else annee, int(trouve[2]), int(trouve[1])).isoformat()
    except ValueError:
        pass
    return None


def journal_suppose(titre):
    titre = sans_accents(titre).lower()
    if "pami" in titre:
        return "pami"
    if "principal" in titre:
        return "robot-principal"
    return None


def type_lien(lien):
    hote = urlparse(lien).netloc
    if "drive.google.com" in hote or "docs.google.com" in hote:
        return "drive"
    if "youtu" in hote:
        return "youtube"
    return "web"


def id_drive(lien):
    trouve = re.search(r"/d/([A-Za-z0-9_-]{20,})", lien) or re.search(r"[?&]id=([A-Za-z0-9_-]{20,})", lien)
    return trouve[1] if trouve else None


def cmd_extraire(args):
    source = Path(args.source)
    sortie = Path(args.sortie) if args.sortie else DOSSIER_TMP
    dossier_images = sortie / "images"
    if dossier_images.exists():
        shutil.rmtree(dossier_images)
    dossier_images.mkdir(parents=True)

    donnees = charger_octets(source)
    images = {}  # chemin dans l'export -> octets
    if donnees[:2] == b"PK":
        try:
            archive = zipfile.ZipFile(io.BytesIO(donnees))
            abime = archive.testzip()
        except zipfile.BadZipFile:
            abime = "archive illisible"
        if abime:
            sys.exit(f"Export ZIP abîmé ({abime}) : le base64 a été tronqué ou mal recopié, refaire l'export.")
        with archive:
            noms_html = [n for n in archive.namelist() if n.lower().endswith((".html", ".htm"))]
            if not noms_html:
                sys.exit("Export ZIP sans page HTML : demander l'export « application/zip » du Google Doc.")
            page = archive.read(noms_html[0]).decode("utf-8", errors="replace")
            for nom in archive.namelist():
                if Path(nom).suffix.lower() in EXT_IMAGES:
                    images[nom] = archive.read(nom)
        lecteur = LecteurHtml()
        lecteur.feed(page)
        lecteur.fermer()
        blocs_bruts = lecteur.blocs
    else:
        texte = donnees.decode("utf-8", errors="replace")
        if re.search(r"<(html|body|p)[\s>]", texte[:5000], re.I):
            lecteur = LecteurHtml()
            lecteur.feed(texte)
            lecteur.fermer()
            blocs_bruts = lecteur.blocs
        else:
            blocs_bruts = blocs_depuis_texte(texte)

    connues = sources_connues()
    ignores = lire_etat().get("ignores", {})
    blocs = []
    section, journal, jour = None, None, None

    def ajouter(bloc):
        identifiant = bloc.get("id")
        if identifiant:
            bloc["logs"] = connues.get(identifiant, [])
            bloc["ignore"] = ignores.get(identifiant)
            bloc["nouveau"] = not bloc["logs"] and identifiant not in ignores
        bloc["index"] = len(blocs) + 1
        blocs.append(bloc)

    for brut in blocs_bruts:
        texte = brut["texte"]
        jour_lu = lire_date(texte) if texte else None
        if jour_lu:
            jour = jour_lu
            ajouter({"type": "date", "texte": texte, "date": jour})
        elif texte and brut["titre"]:
            section, jour = texte, None
            journal = journal_suppose(texte) or journal
            ajouter({"type": "titre", "texte": texte, "journal_suppose": journal})
        elif texte:
            ajouter({
                "type": "texte", "id": hash_texte(texte), "texte": texte,
                "date": jour, "section": section, "journal_suppose": journal,
                "liens": [
                    {"url": lien, "type": type_lien(lien), "id_drive": id_drive(lien)}
                    for lien in brut["liens"]
                ],
            })
        for src in brut["images"]:
            nom = src.split("?")[0]
            octets = images.get(nom) or images.get(nom.lstrip("./"))
            if octets is None:
                ajouter({"type": "image", "source": src, "fichier": None, "date": jour,
                         "section": section, "journal_suppose": journal,
                         "remarque": "image absente de l'export (lien externe ?)"})
                continue
            identifiant = hash_octets(octets)
            fichier = dossier_images / f"{identifiant}{Path(nom).suffix.lower()}"
            fichier.write_bytes(octets)
            ajouter({"type": "image", "id": identifiant, "fichier": fichier.as_posix(),
                     "octets": len(octets), "date": jour, "section": section,
                     "journal_suppose": journal})

    (sortie / "blocs.json").write_text(
        json.dumps(blocs, ensure_ascii=False, indent=2) + "\n", encoding="utf-8"
    )

    contenus = [b for b in blocs if "id" in b]
    nouveaux = [b for b in contenus if b["nouveau"]]
    print(f"{len(blocs)} blocs lus, dont {len(contenus)} de contenu "
          f"({sum(b['type'] == 'image' for b in contenus)} images).")
    print(f"{len(nouveaux)} bloc(s) nouveau(x). Détail complet : {(sortie / 'blocs.json').as_posix()}")
    for bloc in nouveaux:
        contexte = f"date={bloc['date'] or '?'} journal={bloc['journal_suppose'] or '?'}"
        if bloc["type"] == "image":
            print(f"  [{bloc['index']:>3}] {bloc['id']} IMAGE {bloc['fichier']} ({contexte})")
        else:
            apercu = bloc["texte"][:90].replace("\n", " ")
            print(f"  [{bloc['index']:>3}] {bloc['id']} {apercu}... ({contexte})")


# ---------------------------------------------------------------------------
# Fichiers Drive et compression
# ---------------------------------------------------------------------------

def cmd_decoder(args):
    donnees = charger_octets(Path(args.source))
    sortie = Path(args.sortie)
    sortie.parent.mkdir(parents=True, exist_ok=True)
    sortie.write_bytes(donnees)
    print(f"{sortie.as_posix()} : {len(donnees)} octets, id {hash_octets(donnees)}")


def installer(paquet):
    """Installe un paquet Python manquant (machine de la tâche planifiée)."""
    base = [sys.executable, "-m", "pip", "install", "--quiet", paquet]
    for options in ([], ["--user"], ["--break-system-packages"]):
        if subprocess.run(base + options, capture_output=True).returncode == 0:
            return
    sys.exit(f"Impossible d'installer {paquet} : l'installer à la main (pip install {paquet}).")


def charger_pillow():
    try:
        import PIL  # noqa: F401
    except ImportError:
        installer("Pillow")
    from PIL import Image, ImageOps
    return Image, ImageOps


def trouver_ffmpeg():
    chemin = shutil.which("ffmpeg")
    if chemin:
        return chemin
    try:
        import imageio_ffmpeg
    except ImportError:
        installer("imageio-ffmpeg")
        import imageio_ffmpeg
    return imageio_ffmpeg.get_ffmpeg_exe()


def compresser_image(source, cible_sans_ext):
    Image, ImageOps = charger_pillow()
    image = ImageOps.exif_transpose(Image.open(source))
    transparente = image.mode in ("RGBA", "LA") or "transparency" in image.info
    cote = COTE_MAX_IMAGE
    while True:
        reduite = image.copy()
        reduite.thumbnail((cote, cote), Image.LANCZOS)
        if transparente:
            # La transparence impose le PNG (schémas, captures d'écran détourées).
            tampon = io.BytesIO()
            reduite.save(tampon, "PNG", optimize=True)
            if tampon.tell() < TAILLE_MAX:
                return ecrire(cible_sans_ext.with_suffix(".png"), tampon.getvalue())
        else:
            for qualite in (82, 72, 62, 50):
                tampon = io.BytesIO()
                reduite.convert("RGB").save(tampon, "JPEG", quality=qualite, optimize=True, progressive=True)
                if tampon.tell() < TAILLE_MAX:
                    return ecrire(cible_sans_ext.with_suffix(".jpg"), tampon.getvalue())
        cote = int(cote * 0.8)
        if cote < 320:
            raise RuntimeError("image impossible à ramener sous 2 Mo")


def ecrire(cible, octets):
    cible.write_bytes(octets)
    return cible


def duree_video(ffmpeg, source):
    sortie = subprocess.run([ffmpeg, "-hide_banner", "-i", str(source)], capture_output=True, text=True, errors="replace").stderr
    trouve = re.search(r"Duration:\s*(\d+):(\d+):(\d+(?:\.\d+)?)", sortie)
    if not trouve:
        raise RuntimeError("durée de la vidéo illisible")
    return int(trouve[1]) * 3600 + int(trouve[2]) * 60 + float(trouve[3])


def compresser_video(source, cible_sans_ext):
    ffmpeg = trouver_ffmpeg()
    duree = max(duree_video(ffmpeg, source), 0.5)
    cible = cible_sans_ext.with_suffix(".mp4")
    budget = TAILLE_MAX * 0.93 * 8 / duree / 1000  # kbit/s, marge pour le conteneur
    for _ in range(5):
        audio = 48 if budget >= 400 else (32 if budget >= 200 else 0)
        video = budget - audio
        if video < 90:
            raise RuntimeError(
                f"vidéo trop longue ({duree:.0f} s) pour tenir sous 2 Mo avec une image lisible : "
                "la raccourcir ou la laisser en lien"
            )
        hauteur, cadence = (720, 30) if video >= 1100 else (480, 30) if video >= 450 else (360, 24) if video >= 200 else (240, 15)
        commande = [
            ffmpeg, "-hide_banner", "-loglevel", "error", "-y", "-i", str(source),
            "-vf", f"scale=-2:'min({hauteur},ih)',fps={cadence}",
            "-c:v", "libx264", "-preset", "slow", "-pix_fmt", "yuv420p",
            "-b:v", f"{video:.0f}k", "-maxrate", f"{video * 1.3:.0f}k", "-bufsize", f"{video * 2:.0f}k",
            "-movflags", "+faststart",
        ]
        commande += ["-c:a", "aac", "-b:a", f"{audio}k", "-ac", "1"] if audio else ["-an"]
        resultat = subprocess.run(commande + [str(cible)], capture_output=True, text=True, errors="replace")
        if resultat.returncode != 0:
            raise RuntimeError("ffmpeg : " + resultat.stderr.strip()[-300:])
        if cible.stat().st_size < TAILLE_MAX:
            return cible
        budget *= 0.85 * TAILLE_MAX / cible.stat().st_size
    cible.unlink(missing_ok=True)
    raise RuntimeError("vidéo impossible à ramener sous 2 Mo")


def cmd_compresser(args):
    dossier = DOSSIER_JOURNAUX / args.journal / "medias"
    dossier.mkdir(parents=True, exist_ok=True)
    resultats, echec = [], False
    for nom in args.fichiers:
        source = Path(nom)
        identifiant = hash_octets(source.read_bytes())
        cible = dossier / f"{args.prefixe}-{identifiant}"
        extension = source.suffix.lower()
        try:
            if extension in EXT_VIDEOS:
                produit = compresser_video(source, cible)
            elif extension == ".gif" and source.stat().st_size < TAILLE_MAX:
                produit = ecrire(cible.with_suffix(".gif"), source.read_bytes())  # animation conservée
            elif extension in EXT_IMAGES:
                produit = compresser_image(source, cible)
            else:
                raise RuntimeError(f"type de fichier non géré ({extension or 'sans extension'})")
            resultats.append({"source": nom, "id": identifiant, "fichier": produit.name,
                              "octets": produit.stat().st_size})
        except Exception as erreur:  # un média en échec ne doit pas bloquer les autres
            echec = True
            resultats.append({"source": nom, "id": identifiant, "erreur": str(erreur)})
    print(json.dumps(resultats, ensure_ascii=False, indent=2))
    sys.exit(1 if echec else 0)


# ---------------------------------------------------------------------------
# Petites commandes
# ---------------------------------------------------------------------------

def cmd_ignorer(args):
    etat = lire_etat()
    etat.setdefault("ignores", {})[args.id] = args.raison
    ecrire_etat(etat)
    print(f"{args.id} ignoré : {args.raison}")


def cmd_hash(args):
    chemin = Path(args.valeur)
    if args.fichier:
        print(hash_octets(chemin.read_bytes()))
    else:
        print(hash_texte(args.valeur))


def cmd_verifier(args):
    erreurs, alertes = [], []
    for journal in JOURNAUX:
        dossier_medias = DOSSIER_JOURNAUX / journal / "medias"
        cites, numeros = set(), set()
        for chemin in fichiers_logs(journal):
            nom = f"{journal}/logs/{chemin.name}"
            forme = RE_NOM_LOG.match(chemin.name)
            if not forme:
                erreurs.append(f"{nom} : nom attendu AAAA-MM-JJ-NN-titre-court.md (minuscules, sans accents)")
                continue
            if forme[0][:13] in numeros:
                erreurs.append(f"{nom} : numéro d'ordre déjà pris pour cette date")
            numeros.add(forme[0][:13])
            entete, corps = lire_entete(chemin)
            if entete is None:
                erreurs.append(f"{nom} : en-tête YAML absent ou mal fermé")
                continue
            for cle in ("title", "date", "sujet", "description", "sources"):
                if not entete.get(cle):
                    erreurs.append(f"{nom} : champ « {cle} » manquant ou vide")
            if entete.get("date") and entete["date"] != forme[1]:
                erreurs.append(f"{nom} : la date de l'en-tête ({entete['date']}) diffère de celle du nom")
            if len(entete.get("description", "")) > 220:
                alertes.append(f"{nom} : description longue ({len(entete['description'])} caractères), viser 1 à 2 phrases")
            if not RE_HASH.search(entete.get("sources", "")):
                erreurs.append(f"{nom} : « sources » ne contient aucun identifiant de bloc")
            for interdit in ("layout", "parent", "grand_parent", "nav_exclude", "journal"):
                if interdit in entete:
                    alertes.append(f"{nom} : « {interdit} » est déjà fixé par _config.yml, à retirer")
            texte = "\n".join(corps)
            if re.search(r"^# ", texte, re.M):
                alertes.append(f"{nom} : pas de titre « # » dans le corps, le titre vient de l'en-tête")
            if re.search(r"googleusercontent\.com|!\[[^\]]*\]\(https?://", texte):
                erreurs.append(f"{nom} : image liée à un site externe, elle doit être dans medias/")
            medias = RE_INCLUDE_MEDIA.findall(texte)
            if entete.get("vignette"):
                medias.append(entete["vignette"])
            for media in medias:
                cites.add(media)
                if not (dossier_medias / media).is_file():
                    erreurs.append(f"{nom} : média introuvable dans {journal}/medias/ : {media}")
        for fichier in sorted(dossier_medias.iterdir()) if dossier_medias.is_dir() else []:
            if fichier.name.startswith("."):
                continue
            if fichier.stat().st_size >= TAILLE_MAX:
                erreurs.append(f"{journal}/medias/{fichier.name} : {fichier.stat().st_size / 1e6:.2f} Mo, limite 2 Mo")
            if fichier.name not in cites:
                alertes.append(f"{journal}/medias/{fichier.name} : utilisé par aucun log")
    for message in alertes:
        print("ALERTE  " + message)
    for message in erreurs:
        print("ERREUR  " + message)
    nombre = sum(1 for _ in fichiers_logs())
    print(f"{nombre} log(s) contrôlé(s) : {len(erreurs)} erreur(s), {len(alertes)} alerte(s).")
    sys.exit(1 if erreurs else 0)


def main():
    # Les messages sont en français : la console Windows n'est pas en UTF-8 par défaut.
    for flux in (sys.stdout, sys.stderr):
        if hasattr(flux, "reconfigure"):
            flux.reconfigure(encoding="utf-8", errors="replace")

    analyseur = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    commandes = analyseur.add_subparsers(dest="commande", required=True)

    c = commandes.add_parser("extraire", help="liste les blocs d'un export du Google Doc")
    c.add_argument("source", help="export ZIP/HTML/texte, ou résultat d'outil JSON contenant du base64")
    c.add_argument("--sortie", help=f"dossier de travail (défaut : {DOSSIER_TMP.name}/)")
    c.set_defaults(action=cmd_extraire)

    c = commandes.add_parser("decoder", help="écrit sur disque un fichier reçu en base64")
    c.add_argument("source")
    c.add_argument("sortie")
    c.set_defaults(action=cmd_decoder)

    c = commandes.add_parser("compresser", help="compresse des médias sous 2 Mo dans medias/")
    c.add_argument("--journal", required=True, choices=JOURNAUX)
    c.add_argument("--prefixe", required=True, help="début du nom, ex. 2026-08-10-chassis")
    c.add_argument("fichiers", nargs="+")
    c.set_defaults(action=cmd_compresser)

    c = commandes.add_parser("ignorer", help="marque un bloc comme laissé hors des journaux")
    c.add_argument("id")
    c.add_argument("--raison", required=True)
    c.set_defaults(action=cmd_ignorer)

    c = commandes.add_parser("hash", help="identifiant d'un texte (ou d'un fichier avec --fichier)")
    c.add_argument("valeur")
    c.add_argument("--fichier", action="store_true")
    c.set_defaults(action=cmd_hash)

    c = commandes.add_parser("verifier", help="contrôle les logs et les médias")
    c.set_defaults(action=cmd_verifier)

    args = analyseur.parse_args()
    args.action(args)


if __name__ == "__main__":
    main()
