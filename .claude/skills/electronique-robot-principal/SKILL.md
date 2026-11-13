---
name: electronique-robot-principal
description: Carte électronique du robot principal (KiCad, ecad/robot principal/head V1) — choix des composants, feuilles du schéma (puissance, drivers, actionneurs, IHM, capteurs, Luckfox, USB), brochage de l'ESP32-S3, ce qui reste à faire. À charger pour toute question sur le schéma, le câblage ou les composants du robot principal.
---

# Électronique du robot principal

Projet KiCad : `ecad/robot principal/head V1/head V1.kicad_pro`. Schéma commencé le 2026-11-07, 8 feuilles au 2026-11-13, PCB pas commencé. Logs : `docs/journaux/robot-principal/logs/2026-11-07-01-*` et `2026-11-13-01-*`.

## Lire le schéma sans l'ouvrir

`*.kicad_sch` font des milliers de lignes : ne pas les lire. KiCad 10 est installé (`C:/Program Files/KiCad/10.0/bin/kicad-cli.exe`) :

- netlist (broche → net, valeurs) : `kicad-cli sch export netlist -o <scratchpad>/head.net "ecad/robot principal/head V1/head V1.kicad_sch"` ;
- images des feuilles : `kicad-cli sch export svg` ou `pdf`.

Librairies du projet : `ecad/robot principal/librairies 2k27/`.

## Décisions

- 2026-11-07 : carte custom (même logique que la carte PAMI, skill `electronique-pami`), ESP32-S3-WROOM-1, 4 × TMC2209 SilentStepStick, Luckfox Pico Mini B, lidar LD06 sans pilotage de vitesse (d'après la note, mais un net PWM-lidar existe), codeurs AMT102-V sans index.
- 2026-11-13 : ESP32 version sans PSRAM, deux bus I2C, deux connecteurs LD06 (Luckfox ou ESP32), Dynamixel sur l'UART0, empreinte EZ2209 en alternative au TMC2209 pour chaque moteur (U4, U6, U8, U10, valeur vide dans KiCad).

## Feuilles

| Feuille | Fichier | Rôle et composants |
|:--|:--|:--|
| Principale | `head V1.kicad_sch` | ESP32 (U1), reset/boot, tirages I2C R3/R5 (bus 2) et R7/R8 (bus 1), tirages EN R4/R6 |
| Puissance | `power.kicad_sch` | XT60, fusible 15 A, relais K1 G2RL-1-E commandé par J34 « BAU » (coupe `24VD` → `+24V`), TSR1-2433E et TSR2-2450 sur `24VD` (avant relais), 4 convertisseurs 24 V 5 A (U18, U20 → 12V1/12V2 ; U19, U21 → 5 V puissance), TEN 50-2411 (U17) en réserve pour les servos, une LED par rail (RN1) |
| Driver | `driver.kicad_sch` | U3, U5, U7, U9 TMC2209 en 24 V, U4, U6, U8, U10 EZ2209 en alternative sur les mêmes signaux ; EN 1 commun aux moteurs 1-2, EN 2 aux moteurs 3-4 ; connecteurs J2 à J5 |
| Actionneurs | `actionneurs.kicad_sch` | U13 PCA9685 → 16 servos (J20 à J23, rail 5VP1) ; U14 PCA9685 → 8 pompes (RZ7889, IC1 à IC8) + 8 électrovannes (AO3400A + SS14), connecteurs J26 à J33 ; OE commun ; IC9 SN74LVC2G241 → Dynamixel J24/J25 en 12V2 |
| IHM | `IHM.kicad_sch` | U12 MCP23008 (bus I2C 1) : boutons team (choix d'équipe), init (initialisation de la position), lds (activer/désactiver le lidar), bt1, bt2, tirette, reset écran (GP0), OE actionneurs (GP6) ; écran LCD J18 en SPI ; LED WS2812B D3 + J19 |
| Capteurs | `capteurs.kicad_sch` | U11 BOB-12009 (codeurs 5 V → 3,3 V), J8/J9 codeurs, J10 LD06 → Luckfox (TX-LIDAR), J11 LD06 → ESP32 (TX-ESP) |
| Luckfox | `MPUandAttached.kicad_sch` | J7 Luckfox, D2 Schottky sur 5 V, UART3 via R12 + 1 k, TX-LIDAR sur IO_D2 via R11, J6 debug sur IO_B2/IO_B3 |
| USB | `USB.kicad_sch` | J1 USB-C, USBLC6-2SC6, Schottky VBUS, CC 5,1 k |

## Brochage ESP32 (netlist du 2026-11-13)

| Broche | Signal |
|:--|:--|
| IO0 | boot |
| IO1 / IO2 | SDA1 / SCL1 (IHM) |
| IO4 à IO11 | STEP1, DIR1, STEP2, DIR2, STEP3, DIR3, STEP4, DIR4 |
| IO12 / IO13 | EN 1 / EN 2 |
| IO14 / IO15 | SDA2 / SCL2 (actionneurs) |
| IO16 | DIR-Dyn |
| IO17 / IO18 | UART3 RX / TX (Luckfox) |
| IO19 / IO20 | USB D- / D+ |
| IO21, IO35, IO36, IO37 | écran : CS, SCLK, MOSI, DC |
| IO38 à IO41 | A_L, B_L, A_R, B_R (codeurs) |
| IO42 | PWM-lidar |
| IO47 | TX-ESP (lidar) |
| IO48 | NeoPixel |
| TXD0 / RXD0 | UART1_TX / UART1_RX (Dynamixel) |
| IO3, IO45, IO46 | libres |

## Rails d'alimentation (netlist du 2026-11-13)

U15 → `+3.3V`, U16 → `+5V`, U18 → `+12V1`, U20 → `+12V2`, U19 → `+5VP1` (servos), U21 → `+5VP2` (électrovannes), U17 en réserve sur `+5VP1`. Des erreurs de jonction sur ces sorties ont été corrigées le 2026-11-13 : revérifier par la netlist après chaque modification de `power.kicad_sch`.

## Reste à faire

Routage du PCB.
