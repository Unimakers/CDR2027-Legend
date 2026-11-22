---
title: "Fin du premier routage de la carte du robot principal"
date: 2026-11-22
sujet: Électronique
description: "Le premier routage de la carte est terminé : 100 × 174 mm, 4 couches, plus aucune piste non connectée. Le schéma a été revu en cours de route."
vignette: 2026-11-22-electronique-b8112710ea37.jpg
sources: ["d3e015d411e9", "b8112710ea37", "2d976f886e2d", "bf3e4cd26285", "79e16dd51ea2"]
---
## Bilan

**Résultat** : le premier routage de la carte est terminé. La vérification KiCad ne trouve plus aucune piste non connectée. Les courts-circuits relevés entre le +12V1 et la masse ont été corrigés.

**Mesure** : la carte mesure 100 × 174 mm, en 4 couches, avec le trou central pour le passage des câbles.

La carte est organisée en trois zones : les actionneurs en haut, les drivers de part et d'autre du trou central, la logique et l'entrée de puissance en bas.

{% include media.html fichier="2026-11-22-electronique-b8112710ea37.jpg" legende="Vue d'ensemble de la carte routée : actionneurs en haut, drivers autour du trou central, logique et puissance en bas" %}

## Revue du schéma

Le schéma a été revu pendant le routage, puis le PCB mis à jour :

- découplage ajouté sur les PCA9685 et les régulateurs TSR ;
- résistances ajoutées sur les signaux des servomoteurs ;
- références des composants renommées ;
- le TEN 50-2411 (U17) est passé sur la face arrière.

## Actionneurs

Les deux PCA9685 sont au centre de la zone. Les sorties des pompes et des électrovannes sont réparties de chaque côté, avec leurs connecteurs sur les bords de la carte. Les connecteurs des servomoteurs sont en bas de la zone. L'empreinte du TEN 50-2411, qui peut renforcer l'alimentation des servomoteurs, est au milieu.

{% include media.html fichier="2026-11-22-electronique-2d976f886e2d.jpg" legende="Zone actionneurs : les PCA9685 au centre, les sorties pompes et électrovannes de chaque côté, les connecteurs de servomoteurs en bas" %}

## Drivers

Les quatre modules moteurs sont placés deux par deux de chaque côté du trou central. Chacun garde ses deux empreintes de driver (TMC2209 et EZ2209), son condensateur de 100 µF et son connecteur moteur.

{% include media.html fichier="2026-11-22-electronique-bf3e4cd26285.jpg" legende="Les quatre modules moteurs autour du trou central" %}

## Logique et entrée de puissance

L'ESP32 est en bas à gauche, avec son antenne au bord de la carte, à côté du port USB-C et du Luckfox. L'entrée de la batterie est en bas à droite, avec le relais d'arrêt d'urgence et les régulateurs qui alimentent la logique.

{% include media.html fichier="2026-11-22-electronique-79e16dd51ea2.jpg" legende="Zone logique et entrée de puissance : ESP32, USB-C et Luckfox à gauche, entrée batterie et gestion de l'alimentation de la logique à droite" %}

**À prévoir** : travailler la sérigraphie et la documentation imprimée directement sur la carte, pour faciliter son utilisation.
