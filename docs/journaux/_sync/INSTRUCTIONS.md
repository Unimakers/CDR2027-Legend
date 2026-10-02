# Synchronisation des journaux de bord

Ce fichier est la procédure suivie par la tâche hebdomadaire qui alimente les journaux de bord à partir du Google Doc de notes de l'équipe. Elle sert aussi à qui voudrait ajouter un log à la main.

Le dossier `_sync/` n'est pas publié sur le site (Jekyll ignore les dossiers qui commencent par `_`).

## Le principe

L'équipe prend ses notes en vrac dans un Google Doc : texte, photos, vidéos. Une fois par semaine, la tâche relit **tout** le document et n'ajoute aux journaux que ce qui n'y est pas encore : nouvelles notes, ou photos ajoutées après coup sur un sujet déjà journalisé.

Il y a deux journaux, à ne jamais mélanger :

| Journal | Dossier | Contenu |
|:--|:--|:--|
| PAMI | `docs/journaux/pami/` | tout ce qui concerne les PAMI |
| Robot principal | `docs/journaux/robot-principal/` | tout ce qui concerne le robot principal |

Chaque journal a un dossier `logs/` (une page par log) et un dossier `medias/` (photos et vidéos compressées).

## Les règles qui ne se discutent pas

1. **Ne rien inventer.** Un log ne contient que ce qui est écrit ou montré dans le Google Doc. Pas de date supposée sans le signaler, pas de résultat déduit, pas de chiffre arrondi.
2. **Ne pas réécrire le passé.** Le texte déjà publié d'un log ne se modifie pas. On peut seulement lui ajouter une section ou un média.
3. **Moins de 2 Mo par fichier**, sans exception. Tout média passe par `journal.py compresser`.
4. **Ne jamais modifier le Google Doc.** Il est lu, c'est tout.
5. **Ne toucher qu'à `docs/journaux/`.** Rien d'autre dans le dépôt.
6. **Dans le doute, ne pas publier.** Une note dont on ne sait pas à quel robot elle se rapporte reste hors des journaux et est signalée dans le compte rendu.

## Format d'un log

Un fichier par log : `docs/journaux/<journal>/logs/AAAA-MM-JJ-NN-titre-court.md`

- `AAAA-MM-JJ` : la date du log.
- `NN` : numéro d'ordre dans la journée (`01`, `02`...), dans l'ordre où les sujets apparaissent dans le Google Doc.
- `titre-court` : en minuscules, sans accents, mots séparés par des tirets.

```markdown
---
title: "Refonte complète du châssis, moteurs encastrés"
date: 2026-08-10
sujet: Châssis
description: "Le châssis repart de zéro : moteurs encastrés dans une boîte, plateforme au-dessus des NEMA 17."
vignette: 2026-08-10-chassis-1a2b3c4d5e6f.jpg
sources: ["85ec12de94f3", "9128d147b4c3", "1a2b3c4d5e6f"]
---

## Fixation des moteurs pas à pas

**Avant** : les moteurs étaient vissés à des piliers du châssis.

**Décision** : ils sont maintenant encastrés dans une boîte.

{% include media.html fichier="2026-08-10-chassis-1a2b3c4d5e6f.jpg" legende="Les moteurs dans leur boîte" %}
```

| Champ | Rôle |
|:--|:--|
| `title` | Titre du log, 70 caractères au plus. Il dit ce qui s'est passé, pas seulement le sujet. |
| `date` | Date du log, identique à celle du nom de fichier. |
| `date_estimee` | À mettre à `true` seulement si la date n'est pas écrite dans le Google Doc. Le site affiche alors « ≈ ». |
| `sujet` | Sous-système, en un ou deux mots. Reprendre un sujet déjà utilisé dans le journal quand il convient (Électronique, Firmware, Châssis, Roues, Alimentation, Catapulte, Mécanique...). |
| `description` | Une ou deux phrases, 200 caractères au plus. C'est le texte affiché dans la liste des logs. |
| `vignette` | Facultatif. Nom d'une photo de `medias/` affichée dans la liste des logs. |
| `sources` | Identifiants des blocs du Google Doc (paragraphes et images) dont le log est tiré. |

Ne pas écrire `layout`, `parent`, `nav_exclude` ni `journal` : ils sont fixés pour tout le dossier dans `docs/_config.yml`. Le titre, la date, la description et les liens « précédent / suivant » sont affichés par le gabarit `docs/_layouts/log.html` : le corps du fichier commence directement par le détail, sans titre `#`.

### Médias dans un log

```liquid
{% include media.html fichier="NOM.jpg" legende="Ce qu'on voit sur la photo" %}
```

`fichier` est le nom seul, sans dossier. Une vidéo `.mp4` s'insère de la même façon et s'affiche avec un lecteur. Pour mettre plusieurs médias côte à côte :

```html
<div class="log-galerie">
{% include media.html fichier="A.jpg" legende="Vue de dessus" %}
{% include media.html fichier="B.jpg" legende="Vue de côté" %}
</div>
```

La légende décrit ce qui est visible ou ce que dit la note voisine. Regarder l'image avant d'écrire sa légende.

## Rédaction

- En français, phrases complètes, ton factuel. Corriger les fautes et les abréviations des notes, garder tous les chiffres et les noms de composants.
- Un log court sur un seul sujet n'a pas besoin de sous-titre. S'il couvre plusieurs sujets, une section `##` par sujet.
- Quand la note s'y prête, structurer avec des étiquettes en gras : **Constat**, **Problème**, **Décision**, **Résultat**, **À prévoir**. Ne pas forcer une étiquette qui ne correspond à rien dans la note.
- Pas d'emoji, pas de formule d'introduction ou de conclusion.

## Procédure hebdomadaire

Les commandes se lancent depuis la racine du dépôt. Les fichiers de travail vont dans `docs/journaux/_sync/tmp/`, qui n'est pas suivi par git.

### 1. Récupérer le Google Doc

L'identifiant du document est donné dans la consigne de la tâche (il n'est pas écrit dans ce dépôt).

Avec le connecteur Google Drive, appeler `download_file_content` avec `exportMimeType: "application/zip"`. Le résultat contient l'export (page HTML et dossier d'images) encodé en base64.

- Si le résultat est trop gros pour être affiché et a été enregistré dans un fichier, passer ce fichier tel quel à l'étape 2.
- Sinon, recopier exactement la valeur du champ `content` dans `docs/journaux/_sync/tmp/export.b64`.

Si l'export ZIP échoue ou est signalé comme abîmé à l'étape 2, recommencer une fois. En dernier recours, lire le texte avec `read_file_content`, l'enregistrer dans `tmp/doc.txt` et continuer avec ce fichier : les images ne seront pas traitées cette semaine, et il faut le dire dans le compte rendu.

### 2. Lister ce qui est nouveau

```bash
python3 docs/journaux/_sync/journal.py extraire docs/journaux/_sync/tmp/export.b64
```

La commande découpe le document en blocs, écrit `tmp/blocs.json` et extrait les images dans `tmp/images/`. Chaque bloc de contenu a :

- `id` : son identifiant ;
- `nouveau` : `true` s'il n'est cité par aucun log et n'a pas été écarté ;
- `date` : la date écrite au-dessus de lui dans le document (`null` s'il n'y en a pas) ;
- `journal_suppose` : le journal déduit du titre de section (`null` si le titre ne le dit pas) ;
- `logs` : les logs qui le citent déjà ;
- `liens` : les liens qu'il contient (Drive, YouTube, web).

**S'il n'y a aucun bloc nouveau, s'arrêter là : pas de modification, pas de commit.**

Lire `tmp/blocs.json` en entier, pas seulement les blocs nouveaux : le contexte (titres, dates, blocs voisins) sert à ranger les nouveautés au bon endroit.

### 3. Ranger chaque bloc nouveau

**Quel journal ?** Celui de `journal_suppose`. S'il est `null`, se fier au contenu seulement s'il est sans ambiguïté (la note nomme le robot). Sinon, ne pas l'intégrer et le signaler.

**Quelle date ?** Celle du champ `date`. S'il est `null`, chercher une date écrite dans le paragraphe lui-même. À défaut, prendre la date du jour de la synchronisation avec `date_estimee: true`.

Cas particulier des premières notes : dans le Google Doc, toutes les notes prises avant le 2 octobre 2026 sont sous la seule date « 10 août 2026 ». Elles ont été réparties à la main sur des dates estimées, du 10 août au 1er octobre 2026 (logs marqués `date_estimee: true`). Ces logs ne se redatent pas. Un paragraphe nouveau qui hérite encore du 10 août 2026 alors qu'il est placé après les notes déjà journalisées n'a donc pas de date fiable : le traiter comme un bloc sans date.

**Nouveau log ou log existant ?**

- Un paragraphe sur un sujet et une date qui ont déjà un log dans ce journal : l'ajouter à ce log, dans une nouvelle section ou à la fin de la section concernée.
- Un paragraphe qui reprend, en le modifiant, un paragraphe déjà journalisé (même sujet, texte proche) : n'ajouter au log que l'information réellement nouvelle. S'il n'y en a pas, ajouter seulement l'identifiant à `sources`.
- Une image : elle va dans le log qui cite le paragraphe juste au-dessus d'elle dans le document (à défaut, le plus proche). C'est le cas des photos oubliées, ajoutées plus tard sous une ancienne note.
- Sinon : créer un nouveau log. Regrouper dans un même log les paragraphes nouveaux qui partagent la date et le sujet.

Dans tous les cas, ajouter l'identifiant du bloc au champ `sources` du log. C'est ce qui évite de le reprendre la semaine suivante.

**Ce qui ne va pas dans un journal** (ligne de test, liste de courses, note personnelle) s'écarte explicitement :

```bash
python3 docs/journaux/_sync/journal.py ignorer <id> --raison "liste d'achats, hors journal"
```

### 4. Traiter les médias

**Images du document.** Elles sont dans `tmp/images/`, nommées par leur identifiant.

```bash
python3 docs/journaux/_sync/journal.py compresser --journal pami --prefixe 2026-08-10-chassis docs/journaux/_sync/tmp/images/1a2b3c4d5e6f.png
```

`--prefixe` est la date du log suivie du sujet en un mot. La commande écrit le fichier compressé dans `medias/` et affiche son nom final, à utiliser dans `include media.html`. L'identifiant à mettre dans `sources` est celui de l'image d'origine (champ `id`).

**Vidéos et fichiers liés.** Un Google Doc ne contient pas de vidéo, seulement des liens. Pour chaque lien de type `drive` d'un bloc nouveau :

1. Appeler `get_file_metadata` avec son `id_drive` pour connaître son type et sa taille.
2. Si c'est une image ou une vidéo, la télécharger avec `download_file_content`. Quand le résultat est enregistré dans un fichier, le décoder puis le compresser :

   ```bash
   python3 docs/journaux/_sync/journal.py decoder <fichier-du-résultat> docs/journaux/_sync/tmp/video.mp4
   python3 docs/journaux/_sync/journal.py compresser --journal pami --prefixe 2026-08-10-essai docs/journaux/_sync/tmp/video.mp4
   ```

3. Si le téléchargement est impossible (fichier trop gros, accès refusé) ou si la compression échoue (une vidéo de plus de deux minutes environ ne tient pas sous 2 Mo), **ne pas bloquer la synchronisation** : mettre dans le log un lien vers le fichier (`[Voir la vidéo](adresse)`) et le signaler dans le compte rendu. Ne pas ajouter son identifiant à `sources`.

Les liens YouTube et web restent des liens dans le texte.

### 5. Mettre à jour les points ouverts

La page de chaque journal (`docs/journaux/<journal>/index.md`) se termine par un tableau « Points ouverts ». Le tenir à jour à partir des notes nouvelles : ajouter un sujet resté sans décision, retirer une ligne quand une note tranche la question. Ne rien toucher d'autre dans ces pages.

### 6. Contrôler

```bash
python3 docs/journaux/_sync/journal.py verifier
python3 docs/journaux/_sync/journal.py extraire docs/journaux/_sync/tmp/export.b64
```

`verifier` doit finir sans erreur (les alertes sont à lire et à corriger si elles sont justifiées). La seconde commande ne doit plus lister comme nouveaux que les blocs volontairement laissés de côté à l'étape 3.

Mettre ensuite à jour `docs/journaux/_sync/etat.json` : `derniere_synchro` prend la date du jour (`AAAA-MM-JJ`).

### 7. Publier

Les commits sont faits au nom du mainteneur du dépôt, sans aucune mention d'un outil ou d'un assistant, ni dans l'auteur, ni dans le message (pas de ligne `Co-Authored-By`, pas de « Generated with »).

```bash
git add docs/journaux
git -c user.name="TiTooom" -c user.email="quentin.joly001@gmail.com" \
  commit -m "[DOC] Journaux : synchro du JJ/MM/AAAA" -m "PAMI : 2 nouveaux logs, 1 log complété, 3 photos"
git push origin HEAD:main
```

Le message suit la convention du dépôt (`[DOC] ...`). Si le push sur `main` est refusé, pousser sur une branche `journaux/synchro-AAAA-MM-JJ` et le dire dans le compte rendu. Ne jamais forcer un push.

### 8. Compte rendu

Terminer par un résumé court :

- les logs créés et les logs complétés, par journal ;
- les médias ajoutés, avec leur taille ;
- ce qui a été laissé de côté et pourquoi (note sans robot identifiable, vidéo trop lourde, bloc écarté) ;
- la branche et le commit poussés.

## Ajouter un log à la main

Créer le fichier dans `logs/` au format ci-dessus. Pour un log qui ne vient pas du Google Doc, mettre dans `sources` l'identifiant de son propre titre :

```bash
python3 docs/journaux/_sync/journal.py hash "Le titre du log"
python3 docs/journaux/_sync/journal.py verifier
```

## Dépendances

`journal.py` n'utilise que la bibliothèque standard de Python pour `extraire`, `ignorer`, `hash` et `verifier`. `compresser` a besoin de Pillow (images) et de ffmpeg (vidéos) : s'ils manquent, le script installe `Pillow` et `imageio-ffmpeg` avec `pip`.
