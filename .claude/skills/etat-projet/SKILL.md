---
name: etat-projet
description: État actuel du projet CDR2027 (PAMI et robot principal) — ce qui fonctionne, ce qui ne fonctionne pas, décisions prises, points ouverts, historique mécanique. À charger pour toute question « où en est-on », « est-ce que X marche », « qu'est-ce qui reste à faire », ou avant de proposer une solution déjà essayée.
---

# État du projet

Dernière mise à jour : 2026-11-18 (nouvelle carte PAMI routée, GPIO réaffectés ; 2026-11-17 : 2 couches, MCP23008 retiré) ; reste : 2026-11-03 (depuis le code `fw/`, le schéma KiCad et les journaux jusqu'au 2026-10-01).
Sources : `docs/journaux/pami/logs/`, tableau « Points ouverts » de `docs/journaux/pami/index.md`, code.

## Robot principal

Pas de code. Carte électronique : schéma KiCad commencé le 2026-11-07, 8 feuilles le 2026-11-12 (puissance, drivers, actionneurs, IHM, capteurs, Luckfox, USB) ; routage du PCB commencé le 2026-11-16 (2026-11-18 : logique, moteurs, alimentations et Dynamixel routés, reste la partie actionneurs ; carte d’environ 170 mm avec trou central) ; skill `electronique-robot-principal`. Journal commencé le 2026-10-21 : module encodeur (AMT 102-V, roue codeuse, rails) testé le 2026-10-22, v2 corrigée développée mais pas de test noté. Roulement non démontable à la main, sans correction notée. 2026-11-17 : actionneur de levage des murs (servo FS5115M, réducteur sur crémaillère, rail igus sur MakerBeam) monté en prototype, il coulisse le long du rail ; servo SG90 de rotation des murs seulement positionné ; aspiration et rotation pas encore conçues. STL dans `mcad/robot-principal/actionneur/`. Voir `docs/journaux/robot-principal/`.

## PAMI : ce qui fonctionne

| Élément | Détail | Depuis |
|:--|:--|:--|
| Carte ESP32-S3 v1 | PCB 2 couches, DRC OK, câblée et sertie | 2026-08-29 |
| Firmware en components | un dossier par fonction dans `fw/lib/`, pins dans `pamiboard.h` | 2026-09-03 |
| Odométrie | pas des NEMA + cap du MPU9250 | code |
| MPU au-dessus des NEMA | pas de perturbation magnétique ; mouvement < 1° compté comme drift | 2026-09-25 |
| Stratégies | JSON MOVE/ACTION, stockées en LittleFS, lancées depuis l'interface web | code |
| Interface web + télémétrie | AP Wi-Fi, WebSocket, carte de la table | code |
| OTA | flash sans fil (env `esp32-s3-ota`), robot mis à l'arrêt pendant l'écriture | code |
| NeoPixel | animations d'état non bloquantes | code |
| Liaison Luckfox | UART1 115200, protocole `<NOM:data>`, gestion de fichiers à distance | code |
| Appui au sol | têtes de vis à la place de la bille folle : peu de frottement, pas d'usure | 2026-09-30 |
| Châssis v2 | NEMA encastrés dans une boîte fermée par de l'acrylique : droit et solide | 2026-09-19 |

## PAMI : ce qui ne fonctionne pas / pas fini

| Élément | Problème | Source |
|:--|:--|:--|
| Évitement | **non testé** : les ToF à ±35° frôlent un pilier du châssis et faussent la détection | log 2026-10-01 |
| Moteurs DC | PWM suspect (conflit possible avec les timers 50 Hz des servos). Auto-test temporaire dans `setup()`, option `DC_USE_EXPLICIT_LEDC` dans `DC.cpp` | code, commit « Test PWM Moteur DC » |
| RFM69HCW | `RFM69HCW.cpp/.h` vides : radio non implémentée | code |
| Action `DEPLOY_BRAS` | servo 1 à 180°, valeur provisoire (TODO) | `STRATEGY.cpp` |
| `lib/REGULATION` (PID, rampe trapézoïdale) | écrit mais utilisé nulle part | code |
| Luckfox | `trajectory.py` et `strategy.py` sont des essais (plot, miroir équipe), pas reliés au match | code |
| Batterie | se vide vite sans surconsommation mesurée ; fin de vie supposée | log 2026-09-02 |
| Supports ToF | pas assez solides, vis extérieures inaccessibles | log 2026-09-19 |
| Câble management | trop de câbles | logs 2026-08-29, 2026-09-27 |
| Roues silicone | moule refait, chambres dans le moyeu ; résultat final non rapporté | log 2026-09-14 |

## Écarts connus entre schéma et carte réelle (tranchés le 2026-11-03)

- Pont diviseur : le schéma indique 270 kΩ, mais une autre valeur est soudée (270 kΩ indisponible). C'est `R1`/`R2` dans `pamiboard.h` qui fait foi (200 kΩ / 33 kΩ, soit environ 0 à 23,3 V). La plage suit les résistances soudées : `Vmax = 3,3 V × (R1 + R2) / R2`.
- **2 NEMA / 2 TMC2209 par PAMI.** Le moteur 3 du firmware (`stepper3`, GPIO 11/12) n'est pas câblé.

## Nouvelle carte PAMI : tout sur l'ESP32, PCB routé (2026-11-18)

Le MCP23008 (U10) est supprimé (décision 2026-11-17) : tous les signaux vont directement sur des GPIO de l'ESP32. **Le firmware (`fw/`) est encore celui de l'ancienne carte** (il utilise `fw/lib/MCP23008`, pins `PAMI_MCP_*` de `pamiboard.h`, `mcp_setup_board()` appelée par `main.cpp:260`) et suivra quand la carte sera finie. Brochage complet : skill `electronique-pami`.

**Fait le 2026-11-18** (commit `def6408`) : tous les signaux ont un GPIO, PCB routé (539 pistes, 246 vias, zones GND), placement du buck refait (FB court), schéma redessiné. Rien n'est fabriqué ni testé.

GPIO attribués aux anciens signaux du MCP : RST écran IO37, TEAM_BTN IO13, LAUNCHPULL IO47, EN des TMC2209 IO17, XSHUT0 IO4 (un seul ToF). Autres changements par rapport à la v1 : servos 8/18, STEP1/DIR1 15/16, STEP2/DIR2 14/21, MS1/MS2 9/10, M1_IN1 5, tension (`VCC_CALC`) sur IO6 (ADC1). Libres : IO7 et IO12.

À faire dans le firmware au moment de l'adaptation : mettre à jour `pamiboard.h`, retirer ou remplacer `lib/MCP23008`, adapter `lib/VL53L0X` (1 seul ToF, XSHUT direct sur IO4), activer les pull-up internes de TEAM_BTN et LAUNCHPULL, ne pas utiliser `Serial0` (IO43 = CS écran), garder `correctionFactor` pour la tension.



## Décisions prises

- Batterie traversant le PCB (carte v1).
- Moteurs pas à pas NEMA 17 pour la propulsion (odométrie sur les pas).
- Nouvelle carte PAMI, strict nécessaire, pour gagner de la place (en conception, 2026-09-27). **Passée de 4 à 2 couches le 2026-11-17** : plus simple à router, mais moins de place. Journal du 2026-11-18 : refonte totale, 60 × 50 mm (au lieu de 95 × 95), un seul connecteur ToF à l'avant (plus de ToF à 35°), driver RZ7899 ajouté sous un TMC2209 (vitesse seule), module radio retiré, fixation par 3 vis en triangle à valider. Alimentation simplifiée (diodes SS34 D2/D3 à la place des LTC4412 et PMOS AO4407, buck AP63300 conservé).
- Luckfox Pico Mini B en coprocesseur pour les calculs lourds (path finding).

## Points ouverts

Copie du tableau de `docs/journaux/pami/index.md` (c'est lui qui fait foi) :
câble management ; nouvelle carte PAMI 2 couches (routée) ; supports ToF ; ToF à 35° gênés par un pilier ; support écran +3 mm ; autonomie batterie ; choix de la batterie (courant instantané vs couple NEMA 17, piste : batterie d'appareil photo 7 V USB-C) ; appui sur têtes de vis à valider ; catapulte (ressort chargé à la main, libéré par servo, tir visé ~70° à plus de 40 cm) : premier test le 2026-11-03, portée insuffisante selon les ressorts, encoche de retenue du projectile cassée lors d’un déclenchement manuel (renforcer ou imprimer autrement).

## Historique mécanique (résumé des logs)

08-14 châssis sans murs → 08-18 support batterie compact (buck + MPU) → 08-21 plateforme carte + ToF → 08-25 toit/écran abaissé de 1 cm → 09-06 support batterie rehaussé de 1 cm → 09-10 ToF réorientés et reculés de 5 mm → 09-19 refonte complète, NEMA encastrés → 09-22 plateforme au-dessus des NEMA (3 ToF) → 09-30 bille folle PLA détruite en 5 min.

## Documentation du site

La plupart des pages de `docs/` sont encore le **modèle** du MakerSpace (lorem ipsum, « projet XY »). Seuls les journaux sont rédigés. Voir skill `site-doc`.
