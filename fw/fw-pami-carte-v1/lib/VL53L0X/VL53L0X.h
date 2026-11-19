#pragma once
#include <Arduino.h>

// Nombre de capteurs ToF sur le robot
constexpr uint8_t NUM_TOF_SENSORS = 4;

// Allume les capteurs un par un via le MCP et leur assigne une nouvelle adresse I2C
bool tof_init();

// Renvoie la distance en millimètres du capteur demandé (index de 0 à 3)
// Renvoie 8190 si l'obstacle est trop loin (hors de portée)
uint16_t tof_get_distance(uint8_t sensor_index);