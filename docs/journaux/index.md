---
layout: default
title: Journaux
nav_order: 7
has_children: true
has_toc: false
permalink: /journaux/
---

# Journaux de bord

Les journaux gardent la trace du développement au fil de l'eau : ce qui a été fait, ce qui n'a pas marché, ce qui a été décidé et pourquoi. Le but est de pouvoir relire le cheminement de pensée, y compris les pistes abandonnées. La description du résultat final se trouve dans le reste de la documentation, pas ici.

Il y a un journal par robot.

{% include journal_apercu.html %}

## Comment lire un journal

Chaque journal est une liste de **logs**, du plus récent au plus ancien. Un log a une date, un titre et une courte description. Un clic ouvre la page du log, qui détaille le sujet avec ses photos et ses vidéos.

Un log répond, quand c'est pertinent, à ces questions :

- **Constat** : ce qui a été fait ou observé.
- **Problème** : ce qui ne convenait pas.
- **Décision** : ce qui a été changé, et pourquoi.
- **Résultat** : ce que ça a donné une fois testé.

Les anciens logs ne sont pas réécrits, même si la solution a été abandonnée depuis.

## Comment les journaux sont alimentés

Les notes sont prises en vrac dans un Google Doc partagé par l'équipe : texte, photos et vidéos, sans mise en forme particulière. Une fois par semaine, une tâche automatique relit ce document, y repère ce qui est nouveau, compresse les médias (moins de 2 Mo par fichier) et range le tout dans les logs.

Pour qu'une note arrive dans le bon log, il suffit dans le Google Doc de :

- écrire la date sur une ligne à part au-dessus des notes du jour (par exemple `10 août 2026`) ;
- garder un titre de section qui dit de quel robot il s'agit (`PAMI` ou `Robot principal`).

La procédure complète suivie par la tâche est dans [`docs/journaux/_sync/INSTRUCTIONS.md`](https://github.com/Unimakers/CDR2027-Legend/blob/main/docs/journaux/_sync/INSTRUCTIONS.md).
