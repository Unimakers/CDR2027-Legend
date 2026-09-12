#pragma once
#include <Arduino.h>

// Initialise le moteur de la librairie matérielle
void nema_init();

// Configure la résolution physique (Microstepping via MS1 et MS2)
void nema_set_microstepping(bool ms1_high, bool ms2_high);

// Configure la vitesse (en Pas/seconde) et l'accélération (en Pas/s²)
void nema_set_profile(uint8_t motor_id, uint32_t speed_hz, uint32_t accel);

// Déplacement relatif (ex: +1000 pour avancer de 1000 pas, -500 pour reculer)
void nema_move(uint8_t motor_id, long steps);

// Rotation continue (Vitesse constante sans limite de distance)
void nema_run_forward(uint8_t motor_id);
void nema_run_backward(uint8_t motor_id);

// Arrêt en douceur (respecte la décélération) ou Arrêt d'urgence
void nema_stop(uint8_t motor_id, bool force_stop = false);