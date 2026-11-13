 ---
title: "Schéma de la carte du robot principal : puissance, actionneurs et IHM"
date: 2026-11-13
sujet: Électronique
description: "Le schéma de la carte du robot principal compte maintenant 8 feuilles : puissance, actionneurs et IHM sont ajoutées, le lidar et les drivers sont revus."
vignette: 2026-11-13-electronique-5abfd19e10f0.jpg
sources: ["77470dbad22e", "5abfd19e10f0", "e7bdeab9c509", "2fa1eb8ce4d5", "41eaa9f1caed", "4b6edc407b50", "4b1b7026a367", "0452fbb4f6e4", "88be5278f7b6", "50bd1059b466"]
---
## Changements depuis le 7 novembre

- **Trois nouvelles feuilles** : puissance, actionneurs et IHM. Le schéma compte maintenant 8 feuilles.
- **Deux bus I2C** : le bus 1 (IO1, IO2) dessert l'IHM, le bus 2 (IO14, IO15) les cartes d'actionneurs.
- **Lidar** : la carte reçoit deux connecteurs LD06. L'un envoie ses données au Luckfox, l'autre à l'ESP32 (IO47). L'UART0 de l'ESP32, qui lisait le lidar, sert maintenant aux Dynamixel.
- **Microcontrôleur** : l'ESP32-S3-WROOM-1 est la version sans PSRAM. Les broches IO35 à IO37 servent à l'écran.
- **Drivers** : chaque moteur a une seconde empreinte, pour un EZ2209 en alternative au TMC2209, câblée en parallèle sur les mêmes signaux et le même connecteur. L'empreinte EZ, abandonnée le 7 novembre, revient donc sous cette forme.
- **Luckfox** : diode Schottky sur son entrée 5 V. Le connecteur de debug passe sur un UART dédié (IO_B2, IO_B3), l'UART3 reste réservé à l'ESP32.

## Feuille principale

L'ESP32-S3 pilote tout le robot : drivers, capteurs, IHM et actionneurs. La feuille regroupe ses boutons reset et boot, les résistances de tirage des deux bus I2C et les liaisons vers toutes les autres feuilles.

{% include media.html fichier="2026-11-13-electronique-5abfd19e10f0.jpg" legende="Feuille principale : l'ESP32-S3 et les liaisons vers les 7 autres feuilles" %}

## Puissance

Elle produit toutes les tensions de la carte à partir de la batterie, branchée sur un XT60 et protégée par un fusible de 15 A. Un relais G2RL-1-E commandé par l'arrêt d'urgence (connecteur « BAU ») coupe le 24 V qui alimente les moteurs et les actionneurs. Les régulateurs Traco de la logique (TSR1-2433E pour le 3,3 V, TSR2-2450 pour le 5 V) sont branchés avant ce relais. Quatre convertisseurs 24 V de 5 A fournissent deux rails 12 V et deux rails 5 V de puissance. Un TEN 50-2411 est prévu « au cas où le buck 5 A serait trop faible pour les servomoteurs ». Une LED par rail indique sa présence.

{% include media.html fichier="2026-11-13-electronique-50bd1059b466.jpg" legende="Feuille puissance : XT60, fusible, relais d'arrêt d'urgence, régulateurs et LED témoins" %}

## Drivers

Les 4 TMC2209 commandent les moteurs pas à pas du robot. Chacun a en parallèle une empreinte pour un EZ2209 (U4, U6, U8, U10), qui peut le remplacer sur le même connecteur moteur.

{% include media.html fichier="2026-11-13-electronique-2fa1eb8ce4d5.jpg" legende="Feuille driver : les 4 TMC2209 en haut et les empreintes EZ2209 alternatives en bas" %}

## Actionneurs

Cette feuille commande tout ce qui bouge en dehors des roues. Deux PCA9685, sur le bus I2C 2, génèrent les signaux :

- le premier pilote 16 servomoteurs, sur 4 connecteurs de 4 servos alimentés par un rail 5 V de puissance ;
- le second pilote 8 pompes, chacune à travers un pont en H RZ7889, et 8 électrovannes, chacune à travers un MOSFET AO3400A. Pompe et électrovanne partagent un connecteur « pump+ev ».

Les deux PCA9685 ont une broche OE commune, qui coupe d'un coup toutes leurs sorties.

Les Dynamixel sont sur la même feuille. Un SN74LVC2G241 transforme l'UART de l'ESP32 en bus série un fil, avec une broche de direction (DIR-Dyn). Les deux connecteurs Dynamixel sont alimentés en 12 V.

{% include media.html fichier="2026-11-13-electronique-0452fbb4f6e4.jpg" legende="Feuille actionneurs : les deux PCA9685, les connecteurs de servos et les 8 sorties pompe et électrovanne" %}

{% include media.html fichier="2026-11-13-electronique-88be5278f7b6.jpg" legende="Feuille actionneurs, partie Dynamixel : le SN74LVC2G241 et les deux connecteurs" %}

## IHM

C'est l'interface avec l'équipe avant et pendant le match. Un extenseur d'E/S MCP23008, sur le bus I2C 1, lit les boutons « team » (choix de l'équipe), « init » (initialisation de la position), « lds » (switch qui active ou désactive le lidar), « bt1 », « bt2 » et la tirette (« tir »). Il commande aussi le reset de l'écran et la broche OE des actionneurs. L'écran LCD est relié en SPI à l'ESP32. Une LED NeoPixel WS2812B est sur la carte, avec un connecteur pour en chaîner d'autres.

{% include media.html fichier="2026-11-13-electronique-4b1b7026a367.jpg" legende="Feuille IHM : MCP23008, boutons, tirette, connecteur d'écran et LED NeoPixel" %}

## Capteurs

Elle relie les capteurs de position. Les deux codeurs (gauche et droite) passent par un convertisseur de niveau BOB-12009 qui ramène leurs signaux de 5 V à 3,3 V. Les deux connecteurs LD06 permettent de lire un lidar par le Luckfox (TX-FOX) ou par l'ESP32 (TX-ESP).

{% include media.html fichier="2026-11-13-electronique-4b6edc407b50.jpg" legende="Feuille capteurs : convertisseur de niveau, connecteurs des codeurs et des deux lidars" %}

## Luckfox

Le Luckfox Pico Mini B apporte la puissance de calcul. Il échange avec l'ESP32 par l'UART3 et peut lire un lidar directement. Son connecteur de debug donne accès à sa console.

{% include media.html fichier="2026-11-13-electronique-41eaa9f1caed.jpg" legende="Feuille Luckfox : alimentation protégée, liaison UART3, entrée lidar et connecteur de debug" %}

## USB

Le port USB-C sert à programmer et déboguer l'ESP32. Il n'a pas changé depuis le 7 novembre.

{% include media.html fichier="2026-11-13-electronique-e7bdeab9c509.jpg" legende="Feuille USB : connecteur USB-C et protection USBLC6-2SC6" %}
