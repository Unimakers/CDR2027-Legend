# PCBete

Création d'un PCB pour la robotique.

## Objectif :

Carte électronique ESP32-S3-WROOM-U1 permettant : 
- 2 NEMA 17 (28V 2A max), 2 drivers TMC2209
- 2 moteurs DC (3 à 20V 3A max)
- 2 servomoteurs (5V)
- 4 Capteurs TOF VL53L0X (I2C)
- 2 capteurs AS5600 (I2C)
- 1 écran LCD TFT  (SPI) 
- 1 module radio RFM69HCW (SPI)
- LiDAR 5V (UART0)
- LED Neopixel + sa guirlande (60mA/LED)
- Récupération tension d'alimentation (0 à 23V environ avec le pont 200k/33k soudé ; la plage dépend des résistances réellement soudées, voir `fw/include/pamiboard.h`)
- UART1 disponible sur un connecteur 4 broches (GND, TX, RX, 5V) pour une extension (le Luckfox n'est plus monté sur la carte)
- Bouton d'arrêt d'urgence de puissance (30V 12A max)
- Bouton d'équipe (jaune/bleu)
- Tirette de lancement

## Format (2026-11-04)

- Carte 4 couches de **75 × 75 mm**, angles arrondis R3,5, avec un **trou de Ø 30 mm** pour faire passer les câbles, centré en X et 10 mm sous le centre en Y (centre à 37,5 ; 47,5 depuis le coin haut gauche).
- 4 trous M3 à 4 mm des bords (entraxe 67 × 67 mm).
- Placement : haut = USB-C, LDO, boutons, ESP32, MCP23008 ; sous l'ESP32 = batterie, BAU, mesure de tension, buck 5 V ; côtés = les 2 TMC2209 ; droite = connecteurs capteurs ; bas = écran et radio. Drivers DC sur la face arrière, sous U9.
- **Non routée** (placement seul). Plans posés : GND sur In1, VCC_5V_GLOBAL et VCC_12V sur In2.

## Alimentation

Alimentation possible via PowerBank avec 5V/3A vers la carte en Type-C vers Type-C et/ou batterie LiPo 12V (20V max, si pas driver DC soudé 28V max).

Le 5 V batterie est produit **sur la carte** par un buck AP63300 (3 A, entrée 3,8 à 32 V, après le BAU), qui remplace l'ancien module buck externe et ses deux XT30. Sortie réglée par R16/R17 : 0,8 × (1 + 27k/5,1k) = 5,04 V (valeurs E12, 5,1k déjà utilisée sur la carte). Valeurs à vérifier sur la datasheet avant commande.
