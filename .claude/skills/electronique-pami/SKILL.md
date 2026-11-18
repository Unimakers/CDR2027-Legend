---
name: electronique-pami
description: Cartes électroniques des PAMI (KiCad, ecad/pcb-pami) — l'ancienne carte v1 (en service, historique Git) et la nouvelle carte 2 couches en cours : composants, connecteurs, alimentation, brochage ESP32, ce qui marche / ne marche pas pour chacune. À charger pour toute question sur le PCB, le schéma, le câblage, les tensions ou le choix de composants.
---

# Électronique PAMI

Deux cartes. **Ancienne carte (v1)** : celle des PAMI actuels, pilotée par le firmware `fw/`. **Nouvelle carte** : `ecad/pcb-pami/` dans l'arbre de travail, en conception. Projet KiCad : `ecad/pcb-pami/pcb-pami.kicad_pro` (schéma `pcb-pami.kicad_sch`, titre « ESP32-S3 PAMI »). Cahier des charges de la nouvelle carte : `ecad/pcb-pami/README.md`. Source de vérité du brochage : le schéma pour la nouvelle carte, `fw/include/pamiboard.h` pour la v1.

## Ne pas lire en entier

`*.kicad_pcb`, `*.kicad_sch` (~600 Ko), `PCB-backups/`, `pcb-pami-backups/`, `Footprint/`, `3D/`, `fp-info-cache`. Pour le schéma, chercher (Grep) une référence ou une valeur précise. Pour lister composants et nets : agent `kicad` (netlist via `kicad-cli`, ou script qui extrait les blocs `(symbol (lib_id ...) ... (property "Reference" ...))`). La v1 se relit avec `git show 070d910:ecad/pcb-pami/<fichier>`.

---

# Carte v1 (ancienne, en service)

Versionnée le 2026-09-11 (commit `070d910`, « Premier PCB (DRC OK) »), câblée et sertie le 2026-08-29 selon le log. Routée sur F.Cu et B.Cu (827 pistes, 321 vias, 85 empreintes), contour 95 × 95 mm. Évoluée ensuite en brouillon « V2 » (voir plus bas).

**Fonctions** : ESP32-S3-WROOM-1 ; 2 NEMA 17 (28 V, 2 A) avec 2 TMC2209 ; 2 moteurs DC (3-20 V, 3 A) ; 2 servos 5 V ; 4 ToF VL53L0X et 2 AS5600 en I2C ; écran TFT et RFM69HCW en SPI ; LiDAR 5 V sur UART0 ; NeoPixel (60 mA/LED) ; mesure de tension 0-23 V ; Luckfox Pico Mini B sur UART1 ; arrêt d'urgence de puissance (30 V, 12 A) ; bouton d'équipe ; tirette.

**Composants** : ESP32-S3-WROOM-1, MCP23008 (extenseur I2C), 2 × TMC2209 SilentStepStick, AMS1117-3.3, 2 × LTC4412 (sélection de source), 3 × AO4407A (P-MOS de puissance), WS2812B, USB-C, pont diviseur de mesure.

**Alimentation** : power bank 5 V/3 A en USB-C et/ou LiPo 12 V (20 V max ; 28 V max si le driver DC n'est pas soudé). Choix de la batterie pas tranché (voir `etat-projet`).

**Brochage** (`pamiboard.h`) : I2C1 = GPIO 2/1, I2C2 = 41/42 ; SPI SCLK 35, MOSI 36, MISO 38, DC 37, CS écran 39, CS radio 43, IRQ 40 ; UART1 RX 18 / TX 8, UART0 RX 44 ; STEP/DIR NEMA 13/14, 16/17 (11/12 non câblé) ; MS1 47, MS2 21 ; DC 15/7 et 6/5 ; servos 9, 10 ; NeoPixel 48 ; tension sur GPIO 4. Sur le MCP23008 : RST écran (0), bouton d'équipe (1), tirette (2), EN TMC2209 (3), XSHUT3 à XSHUT0 (4 à 7).

**Ce qui marche** (voir `etat-projet`) : carte câblée et sertie, DRC OK ; 2 NEMA / 2 TMC2209 ; odométrie par les pas + MPU ; NeoPixel ; liaison Luckfox sur UART1.

**Ce qui ne marche pas / à savoir** :
- Pont diviseur : 270 kΩ sur le schéma, autre valeur soudée (200 k/33 k, environ 0 à 23,3 V). Les valeurs réelles sont `R1`/`R2` dans `pamiboard.h` ; plage `3,3 V × (R1 + R2) / R2` ; si une résistance change, mettre à jour `R1`/`R2` puis `correctionFactor` après comparaison au multimètre.
- Moteur 3 (`STEP3`/`DIR3`, GPIO 11/12) non câblé : 2 NEMA seulement.
- Moteurs DC : PWM suspect (conflit possible avec les timers 50 Hz des servos), cause non établie (matériel ou firmware).
- Radio RFM69HCW : câblée sur la carte, mais pas implémentée dans le firmware.
- Trop de câbles (câble management), batterie qui se vide vite.

### Brouillon « V2 » 4 couches (2026-09-27 → 2026-11-17, abandonné)

Carte 4 couches de 90 × 66 puis 75 × 75 mm (trou central Ø 30 mm), « strict nécessaire », avec MCP23008 et LTC4412 conservés et un buck AP63300 ajouté. Placée, routage seulement amorcé, jamais fabriquée. Remplacée par la nouvelle carte 2 couches ; l'historique est dans les commits `02bc9c2`, `f4e166a` et `cf7a3d9`.

---

# Nouvelle carte (2 couches, en cours)

Relevé du **2026-11-18** (agent `kicad`, schéma + PCB routé, commit `def6408`). Le firmware `fw/` est encore celui de la v1 et suivra.

## Décisions

- **2 couches** (F.Cu / B.Cu, 1,6 mm), contour 60 × 50 mm : plus simple à router, moins de place. Pas de plans internes, pas de trou central.
- **Tout sur l'ESP32** : MCP23008 retiré. **Un seul ToF**, un seul driver DC, pas de Luckfox ni de LiDAR. **Mesure de tension conservée** : `VCC_CALC` sur IO6 (ADC1_CH5), pont R14 270 k / R15 33 k + C13, lu côté drain du PMOS du BAU (la valeur tombe à 0 quand le BAU coupe).
- Sélection 5 V par OU de Schottky (décision 2026-11-10) : D2 SS34 (`VCC_5V_USB`) et D3 SS34 (`VCC_5V_BUCK`) vers `VCC_5V_GLOBAL`, à la place des 2 LTC4412 + AO4407A1/A2. L'USB-C ne sert qu'à programmer sans batterie ; D3 est prévue pour être shuntée. En USB seul, le 5 V tombe vers 4,6 V.
- Alimentation : LiPo 12 V (20 V max) sur XT30. Choix de la batterie pas tranché.

## Composants (62 références, 62 empreintes, aucun écart schéma/PCB)

| Réf | Valeur | Rôle |
|:--|:--|:--|
| U6 | ESP32-S3-WROOM-1 (empreinte WROOM-1**U**, antenne U.FL externe) | MCU. Référence exacte (N4/N8/N16, R) non écrite : le schéma note « No PSRAM ». **Ne pas prendre un module R8** (PSRAM octale = IO35/36/37 indisponibles) |
| U8 / U9 | TMC2209 SilentStepStick | NEMA B (STEP2/DIR2, J14) / NEMA A (STEP1/DIR1, J22). MS1 et MS2 partagés |
| U1 | RZ7899 (SOIC-8) | Un driver DC, un seul sens (broches 1 et 3 à GND), entrée IO5 (`M1_IN1`, sans pull-down), moteur sur J12 |
| U4 | AMS1117-3.3 | 3V3 depuis `VCC_5V_GLOBAL` |
| U11 | AP63300WU (symbole AP63200WU) | Buck 12 V → 5 V. L1 6,8 µH (empreinte `L_Coilcraft_XAL5030-XXX` ; la XAL5050-682MEC est prévue, la XAL5030 n'existe pas en 6,8 µH), C22 10 µF 50 V, C23/C24 22 µF, R16 27 k / R17 5,1 k (5,04 V). Sortie `VCC_5V_BUCK`. **Placement refait le 2026-11-18** : C22 collé à VIN, SW court et large (1 mm) vers L1.1, BST/C21 serrés, R16/R17 à côté du FB (nœud FB très court), haut de R16 ramené au pad de C23 par une piste 0,2 mm sur F.Cu contournant L1 et C21 par la gauche. Conseil non appliqué : R16 = 28 k (5,19 V) pour que le buck reste prioritaire sur l'USB |
| AO4407A4 | PMOS | Coupure de puissance par le BAU : source `VCC_12V_IN`, drain `VCC_12V`, grille `BAU+` (J9), R13 10 k |
| D6 | WS2812B | NeoPixel embarqué, DIN via R12 470 Ω depuis GPIO48 |
| D1 | (valeur vide) | LED témoin sur 3V3 avec R4 330 Ω ; bibliothèque `LED:SFH482` (LED IR) : à vérifier |
| SW1 / SW2 | SW_Push | BOOT (GPIO0) / EN de l'ESP32 (R1 10 k, C2 1 µF) |

| Connecteur | Br. | Fonction |
|:--|:-:|:--|
| J1 | 16 | USB-C (USB_P/N sur GPIO20/19, CC1/CC2 avec R2/R3 5,1 k) |
| J6 | 2 | Batterie XT30 (`VCC_12V_IN`) |
| J9 | 2 | BAU (`BAU+`, GND) |
| J12 | 2 | Moteur DC (M1+/M1-) |
| J14 / J22 | 4 / 4 | NEMA B / NEMA A |
| J23 / J26 | 3 / 3 | Servo 1 (IO18) / servo 0 (IO8), 5 V |
| J8 | 3 | NeoPixel externe : 5 V, `NEOPIXEL+`, GND. `NEOPIXEL+` est le DOUT de D6 (chaînage) ; la ligne DATA de l'ESP32 n'y va pas |
| J3 | 8 | Écran SPI : GND, 3V3, SCLK, MOSI, RST, DC, CS, 3V3 |
| ~~J17~~ | | Radio RFM69 **retirée** du schéma et du PCB (2026-11-18). MISO et IRQ n'ont plus de destination |
| **J2** | 6 | **ToF unique** : 5 V, GND, SCL1, SDA1, GPIOX (IO11), XSHUT0 (IO4) |
| MPU1 | 4 | SDA1, SCL1, GND, 5 V |
| J5 / J7 | 2 / 2 | Bouton d'équipe (`TEAM_BTN`) / tirette (`LAUNCHPULL`), vers GND |
| H1, H3, H4 | | 3 trous M3 (H2 absent du schéma et du PCB) |

Pull-ups : R5/R6 4,7 k (SCL1/SDA1, J2), R7/R8 4,7 k (SCL2/SDA2, MPU1 seul). Autres passifs : C4/C6 100 µF (12 V), C11 470 µF (5 V), C5/C7/C9 10 µF, C10/C12/C16/C21 100 nF, C18 100 nF (parasites moteur DC).

## Brochage ESP32 (schéma)

Relevé du 2026-11-18. Les GPIO ont changé par rapport à la v1 : le firmware devra être aligné (`pamiboard.h`).

| Signal | GPIO | Remarque |
|:--|:--|:--|
| BOOT | 0 | strapping, usage correct |
| SDA1 / SCL1 | 2 / 1 | J2 (ToF) |
| SDA2 / SCL2 | 41 / 42 | MPU1 ; JTAG par défaut, OK en GPIO |
| XSHUT0 / GPIOX | 4 / 11 | J2.6 / J2.5 |
| M1_IN1 (driver DC) | 5 | |
| VCC_CALC (mesure tension) | 6 | ADC1_CH5, 270 k / 33 k |
| STEP1 / DIR1 | 15 / 16 | NEMA A (U9) |
| EN des TMC2209 | 17 | U8.16 et U9.16 |
| SERVO1 / SERVO0 | 18 / 8 | J23 / J26 |
| MS1 / MS2 | 9 / 10 | partagés U8 et U9 |
| STEP2 / DIR2 | 14 / 21 | NEMA B (U8) |
| TEAM_BTN / LAUNCHPULL | 13 / 47 | J5 / J7 |
| NeoPixel | 48 | via R12 470 Ω |
| SCLK / MOSI / RST écran | 35 / 36 / 37 | IO35-37 : modules sans PSRAM octale seulement |
| MISO / IRQ | 38 / 40 | sans destination (radio retirée) |
| DC écran / CS écran | 39 / 43 | IO43 = TXD0 |
| UART0_RX- | 44 | label orphelin |
| USB_N / USB_P | 19 / 20 | J1 |
| Libres | 7, 12 | non connectés, sans marqueur NC. IO7 = ADC1 (analogique possible), IO12 = ADC2 (inutilisable avec le Wi-Fi). 3, 45, 46 : strapping, NC |

## Ce qui marche / ne marche pas (nouvelle carte)

**Rien n'est testé** : la carte n'est pas fabriquée.

- Schéma complet : alimentation, ESP32 (tous les signaux reliés à un GPIO), USB-C, 2 TMC2209, driver DC, servos, ToF, MPU, écran, NeoPixel, mesure de tension. Aucun écart de net schéma/PCB.
- **PCB routé** (commit `def6408`) : contour 60 × 50 mm (30,25)-(90,75), coins R3,5 ; 539 pistes (352 F.Cu, 187 B.Cu), 246 vias (186 sur GND), 5 zones (GND F.Cu et B.Cu, VCC_12V, VCC_5V_BUCK, VCC_5V_USB). Seul J1 déborde (normal pour un USB-C).
- Netclasses : Default 0,2 mm ; 12V 1,8 mm ; 5V 1,1 mm ; 5VUSB 0,6 mm ; 3.3V 0,3 mm ; DC MOTOR 0,7 mm ; NEMA 0,75 mm ; GND 0,6 mm ; signaux 0,2 mm. Règles : cuivre/bord 0,5 mm, via min 0,5 mm, trou min 0,3 mm. Vias posés : 0,6/0,3 mm.

## Pour une modification

- Les fichiers `~*.lck` sont des verrous KiCad : la carte est peut-être ouverte, ne pas l'écrire.
- Toute broche changée sur une carte doit l'être dans le firmware correspondant (`pamiboard.h`, et le skill `firmware-pami` si l'API change).
