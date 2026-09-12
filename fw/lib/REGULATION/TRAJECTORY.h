#pragma once

// Génère une rampe d'accélération/décélération (Profil Trapézoïdal)
// Renvoie la vitesse théorique à l'instant T pour nourrir le PID
float trajectory_get_target_speed(float current_pos, float target_pos, float max_speed, float acceleration);