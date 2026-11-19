#pragma once
#include <Arduino.h>

// Initialise la broche de l'ADC
void battery_init();

// Effectue la lecture brute, moyenne le bruit et calcule la tension.
// À appeler à chaque tour de boucle
void battery_update();

// Renvoie la tension de la batterie en Volts
float battery_get_voltage();