#pragma once
#include <Arduino.h>

// Initialise les timers matériels et attache les servos aux broches
void servos_init();

// Règle l'angle d'un servo (0 pour SERVO0, 1 pour SERVO1) | Angle entre 0 et 180
void servos_set_angle(uint8_t servo_id, uint8_t angle);

// Désactive le signal PWM d'un servo pour qu'il devienne "mou" et ne chauffe pas
void servos_detach(uint8_t servo_id);