#include "trajectory.h"
#include <math.h> // Pour la fonction sqrt()

float trajectory_get_target_speed(float current_pos, float target_pos, float max_speed, float acceleration) {
    // Calcul de la distance restante
    float distance_left = target_pos - current_pos;
    float abs_distance = fabs(distance_left);
    
    // Tolérance d'arrivée (ex: si on est à moins de 1 millimètre, on demande l'arrêt)
    if (abs_distance < 0.001f) {
        return 0.0f;
    }

    // Formule physique de décélération : v = √(2 * a * d)
    // Cela crée une belle courbe racine carrée pour freiner en douceur
    float braking_speed = sqrt(2.0f * acceleration * abs_distance);

    // On limite la vitesse à la vitesse maximum autorisée
    float target_speed = (braking_speed < max_speed) ? braking_speed : max_speed;

    // On remet le bon signe (avance ou recul)
    if (distance_left < 0) {
        return -target_speed;
    }
    
    return target_speed;
}