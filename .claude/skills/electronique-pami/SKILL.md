---
name: electronique-pami
description: Carte électronique des PAMI (KiCad, ecad/pcb-pami) — fonctions, composants, alimentation, brochage, limites, nouvelle carte 4 couches. À charger pour toute question sur le PCB, le schéma, le câblage, les tensions ou le choix de composants.
---

# Électronique PAMI

Projet KiCad : `ecad/pcb-pami/pcb-pami.kicad_pro` (schéma `pcb-pami.kicad_sch`, titre « ESP32-S3 PAMI »). Cahier des charges : `ecad/pcb-pami/README.md`.

## Ne pas lire en entier

`*.kicad_pcb`, `*.kicad_sch` (~600 Ko), `PCB-backups/`, `pcb-pami-backups/`, `Footprint/`, `3D/`, `fp-info-cache`. Pour le schéma, chercher (Grep) une référence ou une valeur précise. Pour lister les composants : extraire les blocs `(symbol (lib_id ...) ... (property "Reference" ...) (property "Value" ...))` avec un script.

## Fonctions prévues (README)

ESP32-S3-WROOM-1 ; 2 NEMA 17 (28 V, 2 A) avec 2 TMC2209 ; 2 moteurs DC (3-20 V, 3 A) ; 2 servos 5 V ; 4 ToF VL53L0X et 2 AS5600 en I2C ; écran TFT et RFM69HCW en SPI ; LiDAR 5 V sur UART0 ; NeoPixel (60 mA/LED) ; mesure de tension 0-23 V ; Luckfox Pico Mini B sur UART1 ; arrêt d'urgence de puissance (30 V, 12 A) ; bouton d'équipe ; tirette.

## Composants du schéma

ESP32-S3-WROOM-1, MCP23008 (extenseur I2C), 2 × TMC2209 SilentStepStick, AMS1117-3.3, 2 × LTC4412 (sélection de source), 3 × AO4407A (P-MOS de puissance), WS2812B, USB-C, résistances du diviseur 270 kΩ et 33 kΩ.

## Alimentation

- Power bank 5 V/3 A en USB-C, et/ou LiPo 12 V (20 V max ; 28 V max si le driver DC n'est pas soudé).
- Choix de la batterie pas encore tranché (voir `etat-projet`).

## Brochage

Source unique : `fw/include/pamiboard.h`. Résumé : I2C1 = GPIO 2/1, I2C2 = 41/42 ; SPI SCLK 35, MOSI 36, MISO 38, DC 37, CS écran 39, CS radio 43, IRQ 40 ; UART1 RX 18 / TX 8, UART0 RX 44 ; STEP/DIR NEMA 13/14, 16/17 (11/12 non câblé) ; MS1 47, MS2 21 ; DC 15/7 et 6/5 ; servos 9, 10 ; NeoPixel 48 ; tension sur GPIO 4. Sur le MCP23008 : RST écran, bouton équipe, tirette, EN TMC2209, XSHUT0-3.

## Schéma et carte réelle

- Le schéma n'est pas toujours la carte soudée : le pont diviseur porte 270 kΩ sur le schéma, mais une autre valeur est soudée. Valeurs réelles : `R1`/`R2` dans `pamiboard.h`. Plage mesurée : `3,3 V × (R1 + R2) / R2` (environ 23,3 V avec 200 k/33 k). Si une résistance est changée, mettre à jour `R1`/`R2`, et `correctionFactor` après comparaison au multimètre.
- 2 NEMA par PAMI. Les broches STEP3/DIR3 (11/12) existent dans `pamiboard.h` mais ne sont pas câblées.

## Nouvelle carte (en cours, 2026-09-27)

4 couches, strict nécessaire, pour gagner de la place et simplifier le câblage. Les fichiers ne sont pas encore distincts dans le dépôt : vérifier avec l'équipe quelle version est dans `pcb-pami`.

## Pour une modification

- Toute broche changée sur la carte doit l'être aussi dans `pamiboard.h` (et dans le skill `firmware-pami` si l'API change).
- Les fichiers `~*.lck` sont des verrous KiCad : la carte est peut-être ouverte, ne pas l'écrire.
