---
title: "Actionneur de levage des murs : servo, crémaillère et rail igus"
date: 2026-11-17
sujet: Mécanique
description: "Début de l'actionneur qui lève et baisse les murs : un servo FS5115M entraîne un réducteur sur une crémaillère, guidé par un rail igus. Le prototype coulisse le long du rail."
vignette: 2026-11-17-mecanique-b8f6d52dc5bc.jpg
sources: ["2fa3b742bd50", "80d2c7fdb272", "b8f6d52dc5bc"]
---
**Objectif** : lever et baisser les murs une fois qu'ils ont été aspirés.

**Nouveauté** : la conception de l'actionneur a commencé. Il est monté sur un MakerBeam :

- un servo FS5115M entraîne une roue dentée ;
- cette roue dentée entraîne un réducteur qui coulisse sur la crémaillère et lève l'ensemble ;
- de l'autre côté du MakerBeam, un rail igus et son chariot assurent le glissement de l'ensemble ;
- un servo SG90, sur la face avant, servira à faire tourner les murs. Il est seulement positionné.

**Mesure** : d'après le STL de l'assemblage, le MakerBeam (10 × 10 mm) mesure 300 mm, la crémaillère 150 mm et le rail igus 220 mm.

**Résultat** : le prototype imprimé est monté. Entraîné par le servo FS5115M, l'ensemble coulisse le long du rail.

{% include media.html fichier="2026-11-17-mecanique-80d2c7fdb272.mp4" legende="Le prototype imprimé : entraîné par le servo FS5115M, l'ensemble coulisse le long du rail" %}

**À prévoir** : le système d'aspiration et le système de rotation des murs ne sont pas encore conçus.

Le modèle de l'assemblage est dans le dépôt : [mcad/robot-principal/actionneur/actionneur-cdri5.stl](https://github.com/Unimakers/CDR2027-Legend/blob/main/mcad/robot-principal/actionneur/actionneur-cdri5.stl) (aperçu 3D sur GitHub).
