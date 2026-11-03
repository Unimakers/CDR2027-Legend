---
name: firmware-pami
description: Firmware des PAMI (ESP32-S3, PlatformIO/Arduino, FreeRTOS) et coprocesseur Luckfox (Python). Architecture, tâches, components de fw/lib, pins, build/flash/OTA, réglages de stratégie et d'évitement. À charger pour toute question ou modification dans fw/.
---

# Firmware PAMI

## Build et flash

- PlatformIO, `fw/platformio.ini`. Env par défaut `esp32-s3-devkitc-1` (USB, moniteur 115200).
- OTA : se connecter au Wi-Fi `PAMI Unimakers`, puis env `esp32-s3-ota` (`192.168.4.1`, hostname `pami`).
- L'interface web est dans `fw/data/` (LittleFS) : après modification, **Upload Filesystem Image**.
- Pas de CI firmware (la CI ne construit que le site). Vérifier soi-même que ça compile : `pio run` dans `fw/`.
- Dépendances : ESPAsyncWebServer, ArduinoJson 6, Adafruit MCP23008 / VL53L0X / GFX / ST7735-ST7789, FastAccelStepper, ESP32Servo.

## Architecture

- Un **component** = un dossier `fw/lib/NOM/NOM.cpp + NOM.h`, API en fonctions `nom_action()` (sauf `AS5600Encoder`, `PIDController`, `PrecisionTimer` qui sont des classes).
- **Tous les GPIO** sont dans `fw/include/pamiboard.h`. Ne jamais coder une broche en dur ailleurs.
- `fw/src/main.cpp` : `setup()` initialise, puis 3 tâches FreeRTOS :

| Tâche | Cœur | Période | Rôle |
|:--|:--|:--|:--|
| `TaskControl` | 1 | 20 ms | encodeurs, MPU, odométrie, `strategy_update()` |
| `TaskToF` | 0 | ~55 ms | lit les 4 ToF dans `tof_distances[]` (-1 = rien) |
| `TaskUI` | 0 | 100 ms | batterie, écran, télémétrie web, alerte batterie |
| `loop()` | — | 10 ms | OTA, nettoyage web, réception Luckfox |

- Deux bus I2C protégés par mutex : `Wire` (SDA1/SCL1 : encodeur G, MPU, ToF 0-1, MCP23008) et `Wire1` (encodeur D, ToF 2-3). **Tout accès I2C passe par `mutex_Wire` / `mutex_Wire1`.**
- Pendant une OTA (`ota_is_active()`), toutes les tâches se mettent en pause.

## Components

| Component | Rôle | À savoir |
|:--|:--|:--|
| `MCP23008` | extenseur I2C | EN des TMC2209, XSHUT des ToF, bouton équipe, tirette, RST écran |
| `NEMA` | pas à pas (FastAccelStepper, driver RMT) | 2 moteurs : id 1 = droite, 2 = gauche ; id 3 initialisé mais non câblé sur PAMI ; microstepping MS1/MS2 HIGH |
| `PID/MOTION` | odométrie | `motion_init(58 mm roue, 90 mm voie, 3200 pas/tour)` ; x, y en mm |
| `MPU9250` | cap gyro Z | calibrer robot immobile ; cap de départ 90° (`STRATEGY`) |
| `AS5600` | encodeurs magnétiques | lus, envoyés en télémétrie, **pas utilisés** par l'odométrie |
| `VL53L0X` | 4 ToF | 0 = gauche +35°, 1 = centre, 2 = droite -35°, 3 = autre ; 8190 = hors portée |
| `AVOIDANCE` | évitement | modes OFF / STOP / AVOID ; seul le ToF central déclenche, les latéraux choisissent le côté ; table 2000 × 3000 mm |
| `STRATEGY` | machine à états | voir plus bas |
| `DC` | 2 moteurs DC, -255..255 | PWM suspect, voir `etat-projet` |
| `SERVO` | 2 servos 0..180° | |
| `BATTERY` | tension (diviseur sur GPIO 4, `R1`/`R2` = valeurs soudées) | drivers NEMA activés seulement si > 10 V ; alerte LED < 11 V |
| `NEOPIXEL` | 10 LED, animations | CHARGEMENT = init, RESPIRATION = prêt, BATTEMENT = batterie faible |
| `TFT_LCD` | écran SPI | `screen_draw_dashboard()` |
| `WEB_INTERFACE` | AP Wi-Fi + WebSocket | voir `references/protocoles.md` |
| `OTA`, `TIMER`, `LUCKFOX` | OTA, chrono/charge CPU, UART1 | |
| `REGULATION` | PID + rampe | non utilisé |
| `RFM69HCW` | radio | fichiers vides |

## Stratégie

Liste d'étapes JSON (`MOVE` x, y, theta / `ACTION` cmd, param). Actions : `ATTENTE` (ms), `SERVO_ANGLE`, `DEPLOY_BRAS`, `RETRACT_BRAS`.
États : IDLE → ROTATE_TO_TARGET → WAIT_STOP → DRIVING → ROTATE_FINAL ; OBSTACLE_WAIT si obstacle.
Réglages en tête de `fw/lib/STRATEGY/STRATEGY.cpp` (vitesses, tolérance 3°, timeouts, seuils obstacle 200/240 mm, détours max 5).
Formats d'échange : `references/protocoles.md`.

## Luckfox (`fw/fw-luckfox/`)

- Luckfox Pico Mini B, Linux, `ssh root@172.32.0.93` ; scripts dans `/root/scripts_python` (envoi auto par l'extension SFTP de VS Code, `fw/.vscode/sftp.json`).
- `uart.py` : serveur sur `/dev/ttyS3` 115200, **protégé** (ne peut pas être remplacé à distance).
- `trajectory.py` : lissage de trajectoire + miroir pour l'équipe jaune ; `strategy.py` : essai. Rien n'est encore branché au match.

## Pièges connus

- Bloc « TEST PWM DC » dans `setup()` : temporaire, à retirer une fois le PWM DC résolu.
- Ne pas faire tourner un NEMA à la main pendant `nema_init()` (auto-test).
- Unités : firmware en **mm**, interface web et `strategy.json` du Luckfox en **mètres**.
