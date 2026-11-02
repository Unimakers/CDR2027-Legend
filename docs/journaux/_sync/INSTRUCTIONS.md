# Synchronisation des journaux de bord

Ce fichier est la procédure suivie par la tâche hebdomadaire qui alimente les journaux de bord à partir du Google Form « Journal de bord CDR2027 » que remplit l'équipe. Elle sert aussi à qui voudrait ajouter un log à la main.

Le dossier `_sync/` n'est pas publié sur le site (Jekyll ignore les dossiers qui commencent par `_`).

## Le principe

L'équipe envoie ses notes avec un Google Form : robot concerné, date de l'avancée, sujet, texte écrit en vrac, et des photos, vidéos ou documents joints. Chaque envoi est une ligne de la feuille de réponses. Une fois par semaine, la tâche relit **toute** la feuille et n'ajoute aux journaux que ce qui n'y est pas encore.

Les notes prises avant le 2 octobre 2026 venaient d'un Google Doc. Elles sont déjà toutes dans les logs ; la commande `extraire` sert encore à relire cet ancien document si besoin (voir en fin de fichier).

Il y a deux journaux, à ne jamais mélanger :

| Journal | Dossier | Contenu |
|:--|:--|:--|
| PAMI | `docs/journaux/pami/` | tout ce qui concerne les PAMI |
| Robot principal | `docs/journaux/robot-principal/` | tout ce qui concerne le robot principal |

Chaque journal a un dossier `logs/` (une page par log) et un dossier `medias/` (photos et vidéos compressées).

## Les règles qui ne se discutent pas

1. **Ne rien inventer.** Un log ne contient que ce qui est écrit ou montré dans la note. Pas de date supposée sans le signaler, pas de résultat déduit, pas de chiffre arrondi.
2. **Ne pas réécrire le passé.** Le texte déjà publié d'un log ne se modifie pas. On peut seulement lui ajouter une section ou un média.
3. **Moins de 2 Mo par fichier**, sans exception. Tout média passe par `journal.py compresser`.
4. **Ne jamais modifier le formulaire ni la feuille de réponses.** Ils sont lus, c'est tout. Les fichiers envoyés ne sont mis à la corbeille qu'une fois publiés (étape 8).
5. **Ne toucher qu'à `docs/journaux/`.** Rien d'autre dans le dépôt.
6. **Dans le doute, ne pas publier.** Une note dont on ne sait pas à quel robot elle se rapporte reste hors des journaux et est signalée dans le compte rendu.

## Format d'un log

Un fichier par log : `docs/journaux/<journal>/logs/AAAA-MM-JJ-NN-titre-court.md`

- `AAAA-MM-JJ` : la date du log.
- `NN` : numéro d'ordre dans la journée (`01`, `02`...), dans l'ordre des envois.
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
| `date_estimee` | À mettre à `true` seulement si la note n'a pas de date utilisable. Le site affiche alors « ≈ ». |
| `sujet` | Sous-système, en un ou deux mots. Reprendre un sujet déjà utilisé dans le journal quand il convient (Électronique, Firmware, Châssis, Roues, Alimentation, Catapulte, Mécanique...). |
| `description` | Une ou deux phrases, 200 caractères au plus. C'est le texte affiché dans la liste des logs. |
| `vignette` | Facultatif. Nom d'une photo de `medias/` affichée dans la liste des logs. |
| `sources` | Identifiants des notes du formulaire et des fichiers joints dont le log est tiré (ou des blocs de l'ancien Google Doc). |

Ne pas écrire `layout`, `parent`, `nav_exclude` ni `journal` : ils sont fixés pour tout le dossier dans `docs/_config.yml`. Le titre, la date, la description et les liens « précédent / suivant » sont affichés par le gabarit `docs/_layouts/log.html` : le corps du fichier commence directement par le détail, sans titre `#`.

### Médias dans un log

```liquid
{% include media.html fichier="NOM.jpg" legende="Ce qu'on voit sur la photo" %}
```

`fichier` est le nom seul, sans dossier. Une vidéo `.mp4` s'insère de la même façon et s'affiche avec un lecteur. Un document joint (BOM `.xlsx`, schéma `.pdf`...) s'affiche en lien de téléchargement ; `titre="BOM des PAMI"` remplace alors le nom du fichier. Pour mettre plusieurs médias côte à côte :

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

### 1. Exporter la feuille de réponses

Les identifiants de la feuille et du dossier des fichiers envoyés sont donnés dans la consigne de la tâche (ils ne sont pas écrits dans ce dépôt). Exporter la feuille en CSV avec le connecteur Google Drive (`download_file_content`, `exportMimeType: "text/csv"`). Le résultat contient le CSV encodé en base64 :

- si le résultat a été enregistré dans un fichier, passer ce fichier tel quel à l'étape 2 ;
- sinon, recopier exactement la valeur du champ `content` dans `docs/journaux/_sync/tmp/feuille.b64`.

### 2. Lister ce qui est nouveau

```bash
python3 docs/journaux/_sync/journal.py formulaire docs/journaux/_sync/tmp/feuille.b64
```

La commande écrit `tmp/notes.json`, une entrée par envoi :

- `id` : identifiant de la note, calculé sur l'horodatage, le sujet et le texte ;
- `journal` : `pami` ou `robot-principal`, d'après « Robot concerné » (`null` si la réponse ne le dit pas) ;
- `date` : la date de l'avancée (`AAAA-MM-JJ`), `null` si elle est illisible ;
- `sujet`, `texte` : tels qu'écrits par l'équipe ;
- `fichiers` : les fichiers joints, chacun avec son `id`, son `id_drive`, `logs` et `nouveau` ;
- `nouveau` : `true` si la note n'est citée par aucun log et n'a pas été écartée ; `logs` : les logs qui la citent.

La commande liste aussi les fichiers « ajoutés après coup » : nouveaux, mais joints à une note déjà publiée (réponse modifiée après envoi). Ils vont dans le log existant de cette note.

**S'il n'y a ni note ni fichier nouveau, ne rien modifier et ne pas commiter** (passer directement à l'étape 8).

Une réponse modifiée après coup change de texte, donc d'identifiant : elle réapparaît comme nouvelle. Si un log existe déjà pour ce sujet et cette date, n'y ajouter que l'information réellement nouvelle, puis l'identifiant à `sources`.

### 3. Rédiger chaque note nouvelle

**Quel journal ?** Celui du champ `journal`. S'il est `null`, ne pas publier la note et la signaler.

**Quelle date ?** Celle du champ `date`. Si elle est `null`, prendre la date de l'horodatage avec `date_estimee: true`.

**Nouveau log ou log existant ?** Une note sur un sujet et une date qui ont déjà un log dans ce journal complète ce log (nouvelle section ou fin de section). Sinon, elle crée un log. Deux notes du même jour et du même sujet vont dans le même log.

**Le texte.** Les notes sont écrites en vrac : les reformuler selon la partie « Rédaction » ci-dessus, avec le sujet de la note pour point de départ du titre. Le champ `sujet` du log est le sous-système (une liste de sujets est donnée plus haut), pas le sujet du formulaire recopié.

Dans tous les cas, mettre l'identifiant de la note et ceux de ses fichiers dans `sources`. C'est ce qui évite de les reprendre la semaine suivante.

**Ce qui ne va pas dans un journal** (envoi de test, doublon, note personnelle) s'écarte explicitement :

```bash
python3 docs/journaux/_sync/journal.py ignorer <id> --raison "envoi de test"
```

### 4. Traiter les fichiers joints

Pour chaque fichier nouveau :

1. `get_file_metadata` avec son `id_drive` pour connaître son nom, son type et sa taille.
2. Le télécharger avec `download_file_content`, puis le décoder et le passer à `compresser` :

   ```bash
   python3 docs/journaux/_sync/journal.py decoder <fichier-du-résultat> docs/journaux/_sync/tmp/photo.jpg
   python3 docs/journaux/_sync/journal.py compresser --journal pami --prefixe 2026-11-02-bom docs/journaux/_sync/tmp/photo.jpg
   ```

   Garder l'extension d'origine pour le fichier décodé : c'est elle qui décide du traitement. Les photos et vidéos sont compressées sous 2 Mo ; les documents (`.pdf`, `.xlsx`, `.docx`, `.stl`...) sont copiés tels quels s'ils font moins de 2 Mo. `--prefixe` est la date du log suivie du sujet en un mot. La commande affiche le nom final, à utiliser dans `include media.html`.
3. Regarder chaque photo avant d'écrire sa légende.
4. Si le téléchargement est impossible ou si le fichier ne tient pas sous 2 Mo (vidéo de plus de deux minutes environ, gros document), **ne pas bloquer la synchronisation** : mettre dans le log un lien vers le fichier (`[Voir la vidéo](adresse)`), le signaler dans le compte rendu, et ne pas ajouter son identifiant à `sources`. Ce fichier ne sera pas mis à la corbeille à l'étape 8, puisque le lien en dépend.

### 5. Mettre à jour les points ouverts

La page de chaque journal (`docs/journaux/<journal>/index.md`) se termine par un tableau « Points ouverts ». Le tenir à jour à partir des notes nouvelles : ajouter un sujet resté sans décision, retirer une ligne quand une note tranche la question. Ne rien toucher d'autre dans ces pages.

### 6. Contrôler

```bash
python3 docs/journaux/_sync/journal.py verifier
python3 docs/journaux/_sync/journal.py formulaire docs/journaux/_sync/tmp/feuille.b64
```

`verifier` doit finir sans erreur (les alertes sont à lire et à corriger si elles sont justifiées). La seconde commande ne doit plus lister comme nouveaux que les notes et fichiers volontairement laissés de côté (notes sans robot, fichiers restés en lien).

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

### 8. Libérer la place sur le Drive

Les fichiers envoyés prennent de la place sur le Drive du club. Une fois **publiés sur `main`**, ils sont mis à la corbeille :

1. `git fetch origin` : seul ce qui est sur `origin/main` compte comme publié. Un push refusé ou une branche `journaux/synchro-…` non fusionnée ne compte pas.
2. Relancer `formulaire` : la dernière ligne liste les fichiers déjà publiés (identifiants Drive).
3. Pour chacun, vérifier qu'il est bien dans le dossier des fichiers envoyés du formulaire et que son média est dans `docs/journaux/*/medias/` sur `origin/main`, puis le mettre à la corbeille avec `trash_file`. Jamais de suppression définitive : la corbeille garde le fichier 30 jours.

Ne jamais mettre à la corbeille le formulaire, la feuille de réponses, le dossier lui-même, un fichier resté en lien dans un log, ni rien d'autre du Drive. Dans le doute sur un fichier, le garder.

### 9. Compte rendu

Terminer par un résumé court :

- les logs créés et les logs complétés, par journal ;
- les médias ajoutés, avec leur taille ;
- les fichiers mis à la corbeille, avec la place libérée ;
- ce qui a été laissé de côté et pourquoi (note sans robot identifiable, vidéo trop lourde, bloc écarté) ;
- la branche et le commit poussés.

## Ajouter un log à la main

Créer le fichier dans `logs/` au format ci-dessus. Pour un log qui ne vient pas du formulaire, mettre dans `sources` l'identifiant de son propre titre :

```bash
python3 docs/journaux/_sync/journal.py hash "Le titre du log"
python3 docs/journaux/_sync/journal.py verifier
```

## Ancien Google Doc

Jusqu'au 2 octobre 2026, les notes étaient prises dans un Google Doc, découpé en blocs (paragraphes, images) par `extraire` :

```bash
python3 docs/journaux/_sync/journal.py extraire <export.zip>
```

Les logs tirés de ce document citent dans `sources` les identifiants de ses blocs. Dans ce document, toutes les notes d'avant le 2 octobre 2026 sont sous la seule date « 10 août 2026 » ; elles ont été réparties à la main du 10 août au 1er octobre 2026. Ces logs ne se redatent pas.

## Dépendances

`journal.py` n'utilise que la bibliothèque standard de Python pour `formulaire`, `extraire`, `ignorer`, `hash` et `verifier`. `compresser` a besoin de Pillow (images) et de ffmpeg (vidéos) : s'ils manquent, le script installe `Pillow` et `imageio-ffmpeg` avec `pip`.
