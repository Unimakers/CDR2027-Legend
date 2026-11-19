---
layout: home
nav_order: 1
title: Accueil
permalink: /
---

# Bienvenue sur notre documentation

Cette documentation présente le travail de l'équipe **Unimakers** (MakerSpace Amiens) pour la **Coupe de France de Robotique 2027**, dont le thème est « The Legend of Camelot ». Elle décrit nos robots, nos choix techniques et l'avancement du projet.

[Notre projet sur Onshape](https://cad.onshape.com/documents/a1dd7e986cf35e5710cc0af1/v/c28c25944dc0cfd69f9db5c7/e/192be7f926e66b5b2fba6ecb?renderMode=0&uiState=6afeac0e3c4294782866774f){: .btn .btn-primary .fs-5 .mb-4 .mb-md-0 .mr-2 }
[Notre dépôt GitHub](https://github.com/Unimakers/CDR2027-Legend){: .btn .fs-5 .mb-4 .mb-md-0 }

## À propos du projet

Nous préparons deux types de robots :

- un **robot principal**, qui réalise les actions de jeu sur la table ;
- des **PAMI**, petits robots autonomes qui partent de l'écurie en fin de match.

Le projet est développé au MakerSpace Amiens. Tout le code, les cartes électroniques et cette documentation sont dans le dépôt [Unimakers/CDR2027-Legend](https://github.com/Unimakers/CDR2027-Legend).

## Où en sommes-nous ?

Les journaux de bord gardent la trace du développement, semaine après semaine.

**Robot principal**

- La carte électronique est en cours de routage sous KiCad (schéma commencé le 7 novembre 2026).
- Un module encodeur a été développé et testé.
- Un actionneur de levage des murs est monté en prototype.
- Il n'y a pas encore de code.

**PAMI**

- Une première carte à base d'ESP32-S3 est câblée, avec un firmware organisé en composants : odométrie, stratégies en JSON, interface web, mise à jour sans fil.
- Le châssis a été refait plusieurs fois et une catapulte embarquée a été testée.
- Une nouvelle carte, plus petite (60 × 50 mm, 2 couches), est routée mais ni fabriquée ni testée.
- L'évitement d'obstacles n'est pas encore testé.
- Restent ouverts : gestion des câbles, supports ToF, choix et autonomie de la batterie, validation de l'appui sur les têtes de vis. La liste à jour est dans le [journal des PAMI](journaux/pami/index.md).

## Parcourir la documentation

| Section | Contenu |
|:--|:--|
| [Le projet](projet/index.md) | La compétition, le règlement 2027, l'équipe |
| [Le robot principal](robot-principal/index.md) | Électronique, mécanique, logiciel |
| [Les PAMI](pami/index.md) | Électronique, firmware, mécanique |
| [Journaux de bord](journaux/index.md) | Le détail du développement, log par log |
