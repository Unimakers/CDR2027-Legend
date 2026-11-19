---
layout: default
nav_order: 4
title: Les PAMI
---

# Les PAMI

Les PAMI sont les petits robots autonomes de l'équipe. Le détail du développement est dans le [journal des PAMI](journaux/pami/index.md).

## Électronique

La première carte (v1) repose sur un ESP32-S3. C'est un PCB 2 couches, câblé et serti depuis le 29 août 2026. Elle pilote deux moteurs pas à pas NEMA 17 par des TMC2209, des servomoteurs, des LED NeoPixel et des capteurs de distance ToF. Le coprocesseur Luckfox Pico Mini B est prévu pour les calculs lourds, comme la recherche de chemin.

Une nouvelle carte est en conception. Le journal du 18 novembre 2026 décrit une refonte totale : 60 × 50 mm au lieu de 95 × 95, 2 couches, un seul connecteur ToF à l'avant et un module radio retiré. Elle est routée, mais ni fabriquée ni testée.

## Firmware

Le firmware (ESP32-S3, PlatformIO) est organisé en composants, un dossier par fonction. Il gère :

- l'odométrie, à partir des pas des moteurs et du cap du MPU9250 ;
- des stratégies au format JSON, stockées dans la mémoire du robot et lancées depuis l'interface web ;
- une interface web avec télémétrie en direct et carte de la table ;
- la mise à jour sans fil (OTA) ;
- la liaison série avec le Luckfox.

Ce firmware est celui de l'ancienne carte. Il sera adapté quand la nouvelle carte sera finie.

## Mécanique

Le châssis a été refait plusieurs fois. La version actuelle encastre les NEMA dans une boîte fermée par de l'acrylique, ce qui donne un robot droit et solide. L'appui au sol se fait sur des têtes de vis plutôt que sur une bille folle, qui avait été détruite en 5 minutes.

Une catapulte embarquée (ressort chargé à la main, libéré par un servo) a eu son premier test le 3 novembre 2026 : la portée est insuffisante selon les ressorts, et l'encoche de retenue du projectile s'est cassée.

## Points ouverts

Évitement d'obstacles non testé, gestion des câbles, supports ToF, autonomie de la batterie et choix de la batterie, validation de l'appui sur les têtes de vis. La liste à jour est dans le [journal des PAMI](journaux/pami/index.md).
