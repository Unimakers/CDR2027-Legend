---
title: "Lancement de la carte électronique custom du robot principal"
date: 2026-11-07
sujet: Électronique
description: "Le robot principal aura sa propre carte, conçue sous KiCad sur le même modèle que la carte PAMI 2027. Le microcontrôleur et les principaux blocs du schéma sont choisis."
vignette: 2026-11-07-electronique-c07641434608.jpg
sources: ["40bf08320d02", "c07641434608", "bc306f8b8018", "f5f8e40306fa", "bea68eeb4811", "b567048899e0"]
---
**Constat** : le robot n'a encore aucune électronique. Jusqu'ici, la piste étudiée était d'utiliser des cartes du marché.

**Problème** : une carte du marché ne répond jamais entièrement aux attentes et obligerait à écrire le code de zéro.

**Décision** :

- carte custom sous KiCad, qui reprend la logique de la carte PAMI ;
- microcontrôleur : ESP32-S3-WROOM-1 ;
- 4 drivers pas à pas TMC2209 ; l'empreinte compatible avec les drivers BTT EZ est abandonnée ;
- Luckfox Pico Mini B, un MPU compact qui apporte plus de puissance de calcul ;
- lidar LD06 lu directement par l'ESP32, sans pilotage de la vitesse ;
- codeurs AMT102-V, sans utiliser l'index X. Le lidar et les codeurs sont regroupés dans une feuille hiérarchique « capteurs ».

**Résultat** : le schéma est commencé, en partant du microcontrôleur. Le projet KiCad est dans `ecad/robot principal/head V1/`. L'ESP32 et le Luckfox communiquent par l'UART3 (IO17 et IO18 de l'ESP32), à travers des résistances de 1 kΩ, avec un connecteur de debug pour le Luckfox. Le port USB-C, relié à l'USB de l'ESP32, est protégé par un USBLC6-2SC6, avec une diode Schottky sur VBUS et des résistances CC de 5,1 kΩ.

**À prévoir** : intégrer les extensions I2C, les différents switchs et écrans, ainsi que la partie puissance.

{% include media.html fichier="2026-11-07-electronique-c07641434608.jpg" legende="Feuille principale : ESP32-S3-WROOM-1 et liaisons vers les drivers, le Luckfox, l'USB et les capteurs" %}

{% include media.html fichier="2026-11-07-electronique-bc306f8b8018.jpg" legende="Feuille driver : les 4 TMC2209" %}

{% include media.html fichier="2026-11-07-electronique-f5f8e40306fa.jpg" legende="Feuille USB : connecteur USB-C et protection USBLC6-2SC6" %}

{% include media.html fichier="2026-11-07-electronique-bea68eeb4811.jpg" legende="Feuille Luckfox : Luckfox Pico Mini B, liaison UART3 et connecteur de debug" %}

{% include media.html fichier="2026-11-07-electronique-b567048899e0.jpg" legende="Feuille capteurs : connecteurs des deux codeurs et du lidar LD06" %}
