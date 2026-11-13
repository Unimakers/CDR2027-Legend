---
name: journaux
description: Journaux de bord du projet (docs/journaux, PAMI et robot principal) — ajouter ou compléter un log, synchroniser depuis le Google Form, format, règles de rédaction, médias. À charger dès qu'il faut écrire, corriger ou lire des logs, ou mettre à jour les points ouverts.
---

# Journaux de bord

La procédure complète et la référence de format sont dans `docs/journaux/_sync/INSTRUCTIONS.md`. **La lire avant une synchro hebdomadaire.** Pour un log isolé, ce résumé suffit.

## Règles absolues

1. Ne rien inventer (ni date, ni résultat, ni chiffre arrondi).
2. Ne pas réécrire un log publié : on ajoute seulement une section ou un média.
3. Chaque média fait moins de 2 Mo : `journal.py compresser`.
4. Ne toucher qu'à `docs/journaux/`.
5. Dans le doute sur le robot concerné : ne pas publier, signaler.

## Un log

Fichier `docs/journaux/<pami|robot-principal>/logs/AAAA-MM-JJ-NN-titre-court.md` :

```markdown
---
title: "Ce qui s'est passé (70 caractères max)"
date: AAAA-MM-JJ
sujet: Châssis
description: "Une ou deux phrases, 200 caractères max."
vignette: AAAA-MM-JJ-sujet-xxxxxxxxxxxx.jpg
sources: ["xxxxxxxxxxxx"]
---
**Problème** : ...

**Décision** : ...

{% include media.html fichier="AAAA-MM-JJ-sujet-xxxxxxxxxxxx.jpg" legende="Ce qu'on voit" %}
```

- Ordre des champs : title, date, date_estimee, sujet, description, vignette, sources. Pas de `layout`/`parent` (fixés dans `_config.yml`), pas de titre `#`.
- Sujets : Électronique, Firmware, Châssis, Roues, Alimentation, Catapulte, Mécanique, Capteurs.
- Étiquettes : Constat, Problème, Décision, Résultat, À prévoir, Avant, Objectif, Hypothèse, Piste, Mesure, Conséquence, Nouveauté, À retenir.
- `##` seulement si le log couvre plusieurs sujets. Ton factuel, pas d'emoji ni de formule d'intro.
- Log ajouté à la main : `sources` = `python3 docs/journaux/_sync/journal.py hash "Le titre"`.

## Commandes (`python3 docs/journaux/_sync/journal.py ...`)

`formulaire <feuille.b64>` (notes nouvelles) · `decoder` · `compresser --journal pami --prefixe AAAA-MM-JJ-sujet <fichier>` · `ignorer <id> --raison "..."` · `hash` · `verifier` (toujours en dernier, doit finir sans erreur).
Contrôle de forme complet : commande `/verifier-journaux`.

## Après un log

- Mettre à jour le tableau « Points ouverts » de `docs/journaux/<journal>/index.md`.
- Mettre à jour le skill `etat-projet` si le log change ce qui marche ou non.
- Commit `[DOC] Journaux : ...`, au nom du mainteneur, **sans mention d'assistant** (règle d'INSTRUCTIONS.md).

## État

Synchro par formulaire depuis le 2026-10-02 (`_sync/etat.json` : aucune synchro encore enregistrée). PAMI : 22 logs du 2026-08-10 au 2026-11-03. Robot principal : 4 logs du 2026-10-21 au 2026-11-13.
