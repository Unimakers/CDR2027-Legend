# PCBete

Carte électronique des PAMI (petits robots autonomes) pour la Coupe de France de Robotique 2027.

Ce dossier contient la **nouvelle carte** (2 couches, simplifiée, en cours de conception). L'ancienne carte, celle des PAMI actuels, est dans l'historique Git (commit `070d910`, 2026-09-11) ; sa description est dans `.claude/skills/electronique-pami/SKILL.md`.

## Fonctions de la nouvelle carte (état du schéma, 2026-11-17)

Carte autour d'un ESP32-S3-WROOM-1 (empreinte WROOM-1U, antenne externe) :
- 2 NEMA 17 (28 V, 2 A max), 2 drivers TMC2209 SilentStepStick (U8, U9)
- 1 moteur DC (3 à 20 V, 3 A max) avec un driver RZ7899 (U1), un seul sens de rotation
- 2 servomoteurs 5 V (J23, J26)
- 1 capteur ToF VL53L0X en I2C (J2) et 1 MPU en I2C (MPU1)
- 1 écran TFT en SPI (J3)
- 1 module radio RFM69HCW en SPI (J17, empreinte à créer)
- LED NeoPixel embarquée (D6) et connecteur pour une guirlande (J8, 60 mA/LED)
- Arrêt d'urgence de puissance (30 V, 12 A max) par P-MOS commandé depuis J9
- Bouton d'équipe jaune/bleu (J5) et tirette de lancement (J7)
- USB-C (J1) pour la programmation seulement
- Tout est piloté directement par l'ESP32 : plus d'extenseur d'entrées/sorties (MCP23008)

Retiré par rapport à l'ancienne carte : MCP23008, 2 LTC4412 et leurs P-MOS, 2e driver DC, 3 des 4 ToF, capteurs AS5600, LiDAR (UART0), liaison Luckfox (UART1), mesure de la tension batterie.

## Format

- Carte **2 couches** (F.Cu et B.Cu, 1,6 mm), plus simple à router mais avec moins de place. Plus de plans internes.
- Contour **60 × 50 mm**, angles arrondis R3,5, de (30 ; 25) à (90 ; 75) dans KiCad. Pas de trou central.
- 4 trous M3 prévus (H1 à H4) ; H2 n'est pas encore posé dans le PCB.
- **Non routée** : placement en cours, 17 composants encore posés hors du contour (connecteurs J5, J7, J8, J9, J12, U1, PMOS du BAU, U4, condensateurs). Un second rectangle de 70 × 50 mm tracé sur Edge.Cuts en (25 ; 80) est vide.

## Alimentation

- Batterie LiPo 12 V (20 V max) sur XT30 (J6), puis coupure par le P-MOS AO4407A4 commandé par l'arrêt d'urgence (J9).
- Le 5 V est produit sur la carte par un buck AP63300 (3 A, entrée 3,8 à 32 V, après le BAU). Sortie réglée par R16/R17 : 0,8 × (1 + 27k/5,1k) = 5,04 V. Valeurs à vérifier sur la datasheet avant commande.
- Deux diodes SS34 font un OU entre le 5 V du buck (D3) et le 5 V de l'USB-C (D2) vers `VCC_5V_GLOBAL`. L'USB-C ne sert qu'à programmer sans batterie : en USB seul, le 5 V tombe vers 4,6 V.
- 3,3 V par un AMS1117-3.3 (U4).

## Reste à faire

- Relier à un GPIO de l'ESP32 : reset écran (`RST`), `TEAM_BTN`, `LAUNCHPULL`, `XSHUT0` et `GPIOX` du ToF, `EN` des TMC2209.
- Empreinte de J17 (radio), trou H2, finir le placement, puis router.
- Vérifier la LED D1 (valeur vide), l'empreinte de L1 (XAL5050-682MEC prévue) et la ligne DATA du NeoPixel sur J8.
