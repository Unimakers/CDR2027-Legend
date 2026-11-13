---
name: etat-projet
description: État actuel du projet CDR2027 (PAMI et robot principal) — ce qui fonctionne, ce qui ne fonctionne pas, décisions prises, points ouverts, historique mécanique. À charger pour toute question « où en est-on », « est-ce que X marche », « qu'est-ce qui reste à faire », ou avant de proposer une solution déjà essayée.
---

# État du projet

Dernière mise à jour : 2026-11-03 (depuis le code `fw/`, le schéma KiCad et les journaux jusqu'au 2026-10-01).
Sources : `docs/journaux/pami/logs/`, tableau « Points ouverts » de `docs/journaux/pami/index.md`, code.

## Robot principal

Pas de code. Carte électronique : schéma KiCad commencé le 2026-11-07, 8 feuilles le 2026-11-12 (puissance, drivers, actionneurs, IHM, capteurs, Luckfox, USB) ; PCB à faire ; skill `electronique-robot-principal`. Journal commencé le 2026-10-21 : module encodeur (AMT 102-V, roue codeuse, rails) testé le 2026-10-22, v2 corrigée développée mais pas de test noté. Roulement non démontable à la main, sans correction notée. Voir `docs/journaux/robot-principal/`.

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

## Décisions prises

- Batterie traversant le PCB (carte v1).
- Moteurs pas à pas NEMA 17 pour la propulsion (odométrie sur les pas).
- Nouvelle carte PAMI **4 couches**, strict nécessaire, pour gagner de la place (en conception, 2026-09-27).
- Luckfox Pico Mini B en coprocesseur pour les calculs lourds (path finding).

## Points ouverts

Copie du tableau de `docs/journaux/pami/index.md` (c'est lui qui fait foi) :
câble management ; nouvelle carte 4 couches ; supports ToF ; ToF à 35° gênés par un pilier ; support écran +3 mm ; autonomie batterie ; choix de la batterie (courant instantané vs couple NEMA 17, piste : batterie d'appareil photo 7 V USB-C) ; appui sur têtes de vis à valider ; catapulte (ressort chargé à la main, libéré par servo, tir visé ~70° à plus de 40 cm) : premier test le 2026-11-03, portée insuffisante selon les ressorts, encoche de retenue du projectile cassée lors d’un déclenchement manuel (renforcer ou imprimer autrement).

## Historique mécanique (résumé des logs)

08-14 châssis sans murs → 08-18 support batterie compact (buck + MPU) → 08-21 plateforme carte + ToF → 08-25 toit/écran abaissé de 1 cm → 09-06 support batterie rehaussé de 1 cm → 09-10 ToF réorientés et reculés de 5 mm → 09-19 refonte complète, NEMA encastrés → 09-22 plateforme au-dessus des NEMA (3 ToF) → 09-30 bille folle PLA détruite en 5 min.

## Documentation du site

La plupart des pages de `docs/` sont encore le **modèle** du MakerSpace (lorem ipsum, « projet XY »). Seuls les journaux sont rédigés. Voir skill `site-doc`.
