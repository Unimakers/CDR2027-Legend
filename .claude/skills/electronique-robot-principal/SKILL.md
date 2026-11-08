---
name: electronique-robot-principal
description: Carte électronique du robot principal (KiCad, ecad/robot principal/head V1) — choix des composants, feuilles du schéma, brochage de l'ESP32-S3, liaison Luckfox, capteurs, ce qui reste à faire. À charger pour toute question sur le schéma, le câblage ou les composants du robot principal.
---

# Électronique du robot principal

Projet KiCad : `ecad/robot principal/head V1/head V1.kicad_pro` (schéma commencé le 2026-11-07, PCB vide). Log : `docs/journaux/robot-principal/logs/2026-11-07-01-lancement-de-la-carte-electronique.md`.

## Ne pas lire en entier

`*.kicad_sch` (plusieurs milliers de lignes), `*.kicad_pcb`, `*.step`. Chercher (Grep) une référence ou un label précis. Librairies du projet : `ecad/robot principal/librairies 2k27/` (TMC2209 SilentStepStick, TEN 50-2411, « Yosh's lib »).

## Décisions (2026-11-07)

- Carte custom plutôt que cartes du marché, même logique que la carte PAMI (skill `electronique-pami`).
- ESP32-S3-WROOM-1, 4 × TMC2209 en modules SilentStepStick (empreinte BTT EZ abandonnée).
- Luckfox Pico Mini B comme coprocesseur (calcul).
- Lidar LD06 lu par l'ESP32, sans pilotage de la vitesse (d'après la note).
- Codeurs AMT102-V, index X non utilisé.

## Feuilles du schéma

| Feuille | Fichier | Contenu |
|:--|:--|:--|
| Principale | `head V1.kicad_sch` | ESP32, reset (R2 10 k, C3 1 µF), boot (R1 10 k), pull-up I2C R5/R6 4,7 k, découplage C1 22 µF / C2 100 nF |
| Driver | `driver.kicad_sch` | U3 à U6 TMC2209, 100 µF 35 V + 100 nF chacun, connecteurs J1, J3, J4, J5 |
| USB | `USB.kicad_sch` | J2 USB-C, U2 USBLC6-2SC6, D1 Schottky sur VBUS, R3/R4 5,1 k sur CC |
| Luckfox | `MPUandAttached.kicad_sch` | J8 Luckfox (VBUS en 5 V), IO_D0/IO_D1 vers UART3 via R7/R8 1 k, J6 debug 4 broches (5V, RX, TX, GND) |
| Capteurs | (feuille « capteurs ») | U7 BOB-12009 (convertisseur de niveau 3,3 V ↔ 5 V) pour A/B des codeurs, J9 Encoder_Left, J7 Encoder_Right, J12 lidar LD06, C6/C7 près du lidar, C8/C9 près des codeurs |
| Puissances | à faire | rails +24 V, +5 V, +3,3 V déjà utilisés par les autres feuilles |

## Brochage ESP32 (lu sur le schéma du 2026-11-07)

| Broche | Signal |
|:--|:--|
| IO0 | boot |
| IO1 / IO2 | SDA1 / SCL1 |
| IO4 à IO11 | STEP1, DIR1, STEP2, DIR2, STEP3, DIR3, STEP4, DIR4 |
| IO12 / IO13 | EN 1 / EN 2 (pull-up R10/R11 vers 3,3 V) |
| IO17 / IO18 | UART3_RX / UART3_TX (Luckfox) |
| IO19 / IO20 | USB D- / D+ |
| RXD0 | RX-LIDAR (TX du LD06) |
| IO38 à IO41 | A_L, B_L, A_R, B_R (codeurs) |
| IO42 | PWM-lidar (net câblé, alors que la note dit « sans pilotage de la vitesse ») |

Vérifier dans le `.kicad_sch` avant de s'en servir : le schéma évolue.

## Reste à faire (2026-11-07)

Extensions I2C, switchs, écrans, feuille puissance, routage du PCB.
