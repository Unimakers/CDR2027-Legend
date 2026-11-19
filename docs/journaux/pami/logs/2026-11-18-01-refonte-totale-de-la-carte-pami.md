---
title: "Refonte totale de la carte PAMI : 60x50 mm en 2 couches"
date: 2026-11-18
sujet: Électronique
description: "La carte V1 est abandonnée au profit d'une carte minimale de 60x50 mm au lieu de 95x95 mm, en 2 couches, avec buck intégré et un seul connecteur ToF."
vignette: 2026-11-18-carte-a239634bd61b.jpg
sources: ["af5c9498326d"]
---
**Avant** : jusqu'ici, l'effort portait sur l'optimisation de la V1, surtout en place. Il y a trop de fonctionnalités sur cette carte et la condenser atteint ses limites.

**Décision** : tout reprendre en retirant ce qui n'est pas utile, pour réduire la carte de 95x95 mm à 60x50 mm. Le minimum conservé : 2 moteurs NEMA 17, 1 capteur ToF, 2 servomoteurs, la gestion de la batterie et l'alimentation. Des fonctions supplémentaires s'ajoutent s'il reste de la place.

**Conséquence** : la carte étant beaucoup plus petite, le PCB 4 couches complique la réalisation. Retour à 2 couches.

## Schéma

Le schéma est organisé en blocs : ESP32-S3 minimal (USB-C, boutons BOOT et RESET, régulateur 5 V vers 3,3 V), gestion de l'alimentation, moteurs, communication, entrées/sorties et divers. Le code couleur des cadres indique l'avancement (vert : fait, orange : GPIO à attribuer, rouge : à commencer).

- **Alimentation** : le buck converter, auparavant un module externe, est intégré à la carte et prend de la place. L'entrée 12 V de la batterie passe par le BAU, puis une source 5 V double (deux diodes) choisit entre le 5 V du USB-C et celui du buck. Une LED témoin et le voltage calculator, conservé, complètent le bloc.
- **Moteurs** : deux drivers TMC2209 pour les NEMA 17. Un driver RZ7899, déjà présent sur la V1 mais bridé, est ajouté dans la place restée libre sous un des TMC2209. Il commande un moteur en vitesse uniquement, pas en direction, ce qui économise des GPIO.
- **Capteur** : un seul connecteur ToF (VL53L0X) avec les lignes d'alimentation, I2C, XSHUT et GPIO. À cause de la taille de la carte, qui vise à réduire celle du robot, il n'est plus possible de placer les ToF à 35°, ça prend trop de place. Il n'y a donc plus qu'un ToF à brancher, ce qui réduit fortement les câbles dans le robot et le sertissage, donc du temps gagné.
- **Inchangé** : l'écran LCD (SPI), le MPU (I2C) et la LED NeoPixel.
- **Retiré** : le module radio.
- **Autres blocs** : deux sorties servo, bouton d'équipe, tirette de lancement et trois trous de fixation.

{% include media.html fichier="2026-11-18-carte-1f754d7c1b25.jpg" legende="Schéma de la nouvelle carte, découpé en blocs (ESP32-S3, alimentation, entrées/sorties, moteurs, communication, divers)" %}

## Routage

Le routage est fait sur 2 couches. Les gros câblages d'alimentation sont tracés en larges pistes : l'arrivée du 12 V de la batterie vers le buck et les drivers, et le 5 V global qui fait le tour de la carte pour alimenter l'écran, le MPU et le ToF. Les deux drivers de moteur occupent le bas de la carte, chacun avec son condensateur de puissance, et l'ESP32-S3 est au centre-droit, avec l'écran sur le bord droit. Les connecteurs sont en périphérie.

**Nouveauté** : un nouveau système de fixation a été essayé, avec 3 trous en triangle (un en haut au centre, deux en bas près des condensateurs). L'objectif est de fixer la carte avec une seule vis, celle du milieu, et si ce n'est pas suffisant, d'utiliser les deux autres emplacements.

{% include media.html fichier="2026-11-18-carte-cbc29a233acf.jpg" legende="Routage de la carte 2 couches : pistes d'alimentation larges, deux drivers en bas, ESP32-S3 et écran à droite, trois trous de fixation en triangle" %}

## Vue 3D

La vue 3D montre l'agencement final des composants : USB-C et buck 5 V / 3 A à gauche, boutons RST et BOOT en haut, connecteur ToF en haut (côté avant), écran à droite, les connecteurs des moteurs A et B, BP0 et BP1, la sortie LED NEO et les deux servos en bas. Le driver RZ7899 est visible sous le driver de gauche, le driver de droite est seul.

{% include media.html fichier="2026-11-18-carte-a239634bd61b.jpg" legende="Vue 3D de la carte : buck et USB-C à gauche, ESP32-S3 à droite, drivers moteurs et connecteurs en bas" %}
