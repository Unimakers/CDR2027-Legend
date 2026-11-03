---
description: Vérifie la mise en forme des journaux de bord (PAMI et robot principal) et propose les corrections
argument-hint: "[pami | robot-principal | chemin/vers/un-log.md]"
allowed-tools: Read, Glob, Grep, Bash(python docs/journaux/_sync/journal.py verifier), Bash(python3 docs/journaux/_sync/journal.py verifier), Bash(git status:*), Bash(git log:*), Bash(git diff:*)
---

Vérifie la mise en forme des journaux de bord dans `docs/journaux/`. La référence est `docs/journaux/_sync/INSTRUCTIONS.md` (parties « Format d'un log », « Médias dans un log » et « Rédaction ») : la relire avant de commencer.

## Portée

- Sans argument : tous les logs de `docs/journaux/pami/logs/` et `docs/journaux/robot-principal/logs/`.
- `pami` ou `robot-principal` : ce journal seulement.
- Un chemin de fichier : ce log seulement.

Argument reçu : `$ARGUMENTS`

## Étape 1 : contrôle automatique

Lancer `python docs/journaux/_sync/journal.py verifier` (ou `python3`). Reprendre ses erreurs et alertes telles quelles dans le rapport. Elles couvrent déjà : nom du fichier, numéro d'ordre, en-tête YAML, champs obligatoires, date du nom = date de l'en-tête, champs interdits, titre `#` dans le corps, images externes, médias introuvables ou inutilisés, taille sous 2 Mo.

## Étape 2 : contrôles de forme

Lire chaque log de la portée en entier et vérifier les points suivants. Chaque écart est une **erreur**.

**En-tête**
- `title` : 70 caractères au plus, entre guillemets doubles. Il dit ce qui s'est passé, pas seulement le sujet (« Connecteur NEMA cassé », pas « Connecteur NEMA »).
- `description` : 200 caractères au plus, entre guillemets doubles, une ou deux phrases complètes terminées par un point.
- `sujet` : un ou deux mots, sans guillemets. Doit être un sujet déjà utilisé dans le même journal ou un de la liste d'INSTRUCTIONS.md (Électronique, Firmware, Châssis, Roues, Alimentation, Catapulte, Mécanique, Capteurs). Un sujet nouveau, ou une variante d'un sujet existant (« Chassis », « Électronique PCB »), est signalé.
- `sources` : liste JSON sur une ligne, identifiants de 12 caractères hexadécimaux entre guillemets doubles, sans doublon.
- `vignette` (facultatif) : nom de fichier seul, sans dossier, et la même photo est aussi insérée dans le corps du log.
- `date_estimee` : absent, ou exactement `true`.
- Ordre des champs : `title`, `date`, `date_estimee`, `sujet`, `description`, `vignette`, `sources`.

**Corps**
- Commence juste après le `---` de fermeture, sans ligne de titre.
- Sous-titres en `##` uniquement (pas de `###` ni plus), et seulement si le log couvre plusieurs sujets. Un log avec une seule section `##` est signalé.
- Étiquettes en gras sous la forme exacte `**Étiquette** : texte` (espace avant et après les deux-points, les deux-points hors du gras), en début de paragraphe.
- Étiquettes autorisées : Constat, Problème, Décision, Résultat, À prévoir, Avant, Objectif, Hypothèse, Piste, Mesure, Conséquence, Nouveauté, À retenir. Toute autre est signalée avec un remplacement : « À faire » → « À prévoir », « Crainte » → « Hypothèse », « Critère de décision » → « Objectif » ou « Hypothèse » selon le sens.
- Médias : uniquement par `{% include media.html fichier="..." legende="..." %}`, nom seul sans dossier, avec une `legende` non vide (ou un `titre` pour un document joint). Plusieurs médias côte à côte vont dans `<div class="log-galerie">`.
- Pas de syntaxe Markdown d'image `![](...)`, pas de HTML autre que `<div class="log-galerie">`.
- Une ligne vide entre deux paragraphes, entre deux étiquettes, et autour de chaque `include` ou `##`. Le fichier finit par un seul retour à la ligne, sans espaces en fin de ligne.

## Étape 3 : contrôles de rédaction

Chaque écart est une **alerte** (à juger, pas forcément à corriger).

- Fautes d'orthographe, de grammaire, d'accord, accents manquants (y compris dans `title` et `description`).
- Abréviations ou style de note : « pb », « qd », « +/- », mots manquants, phrases sans verbe.
- Emoji, formule d'introduction (« Aujourd'hui on a... ») ou de conclusion (« Affaire à suivre »).
- Ton : factuel, pas d'exclamation, pas d'avis non tiré de la note.
- Légende vague (« Photo », « Image 1 ») ou qui ne décrit pas ce qu'on voit. Regarder l'image avec Read avant de juger une légende.
- Étiquette qui ne correspond pas à son contenu (un **Résultat** qui est en fait une piste).
- Description qui ne résume pas le log, ou titre et description qui se répètent mot pour mot.
- Noms de composants écrits de plusieurs façons dans le journal (« ToF » / « TOF », « NEMA 17 » / « Nema17 ») : signaler la forme majoritaire.

Ne jamais signaler comme faute un chiffre ou un nom de composant : la règle est de les garder tels quels.

## Étape 4 : rapport

Ne rien modifier pendant les étapes 1 à 3. Rendre un rapport court, en français :

1. Une ligne de bilan : nombre de logs vérifiés, d'erreurs, d'alertes.
2. Les écarts groupés par fichier (lien cliquable vers le fichier et la ligne), erreurs d'abord. Pour chacun : ce qui ne va pas, et la correction proposée (le texte exact à mettre).
3. Les fichiers sans écart ne sont pas listés un par un.

Indiquer pour chaque correction si elle touche au **texte publié** (corps d'un log déjà présent sur `origin/main`, voir `git log origin/main -- <fichier>`). INSTRUCTIONS.md interdit de réécrire le passé : ces corrections-là sont à valider une par une.

## Étape 5 : corrections

Demander quelles corrections appliquer (toutes, seulement les erreurs, une liste, aucune). N'appliquer que celles validées, avec Edit, en ne touchant qu'à `docs/journaux/`. Relancer ensuite `journal.py verifier` pour confirmer. Ne pas commiter.
