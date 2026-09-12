#pragma once
#include <Arduino.h>

// Constantes physiques de ton robot
constexpr float WHEEL_RADIUS_M = 0.03f; // 3 cm = 0.03 mètres

// Initialiser ou réinitialiser la position du robot
void odometry_init(float start_x = 0.0f, float start_y = 0.0f, float start_theta = 0.0f);

// Fonction mathématique pure (à appeler à chaque boucle)
void odometry_update(float current_angle_l, float current_angle_r, float gyro_z_dps, float delta_t_sec);

// Récupérer la position
float odometry_get_x();
float odometry_get_y();
float odometry_get_theta_rad(); // Cap en radians (pour les maths)
float odometry_get_theta_deg(); // Cap en degrés (pour l'affichage humain)