# PCBete

Création d'un PCB pour la robotique.

## Objectif :

Carte électronique ESP32-S3-WROOM-U1 permettant : 
- Jusqu'à 3 NEMA 17 (28V 2A max)
- 2 moteurs DC (3 à 20V 3A max)
- 2 servomoteurs (5V)
- 4 Capteurs TOF VL53L0X (I2C)
- 2 capteurs AS5600 (I2C)
- 1 écran LCD TFT  (SPI) 
- 1 module radio RFM69HCW (SPI)
- LiDAR 5V (UART0)
- LED Neopixel + sa guirlande (60mA/LED)
- Récuparation tension d'alimentation (0 à 23V)
- Coopération Master/Slave avec Luckfox Pico Mini B (UART1)
- Bouton d'arrêt d'urgence de puissance (30V 12A max)
- Bouton d'équipe (jaune/bleu)
- Tirette de lancement

## Alimentation

Alimentation possible via PowerBank avec 5V/3A vers la carte en Type-C vers Type-C et/ou batterie LiPo 12V (20V max, si pas driver DC soudé 28V max)

## Luckfox

Micro ordinateur utilisant un noyau Linux capable d'executer du Python cadencé à 1.2Ghz. Afin de décharger l'ESP32-S3 des calculs lourds (path finding, trajctory planner, etc). Il se fixe en direct sur le PCB, se reliant ainsi en UART1 avec l'ESP32 permettant la communication.



