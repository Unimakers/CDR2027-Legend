#include "ODOMETRY.h"
#include <math.h> // Pour cos(), sin(), PI

static float pos_x = 0.0f;
static float pos_y = 0.0f;
static float theta_rad = 0.0f;

static float last_angle_l = 0.0f;
static float last_angle_r = 0.0f;
static bool first_update = true;

void odometry_init(float start_x, float start_y, float start_theta) {
    pos_x = start_x;
    pos_y = start_y;
    theta_rad = start_theta;
    first_update = true; // Forcera la mémorisation au prochain update
}

void odometry_update(float current_angle_l, float current_angle_r, float gyro_z_dps, float delta_t_sec) {
    // On se contente de mémoriser l'état initial
    if (first_update) {
        last_angle_l = current_angle_l;
        last_angle_r = current_angle_r;
        first_update = false;
        return; 
    }

    // Calcul des deltas d'angles des roues (en degrés)
    float delta_deg_l = current_angle_l - last_angle_l;
    float delta_deg_r = current_angle_r - last_angle_r;

    // Conversion en distance parcourue (mètres)
    float perimeter = 2.0f * PI * WHEEL_RADIUS_M;
    float dist_l = (delta_deg_l / 360.0f) * perimeter;
    float dist_r = (delta_deg_r / 360.0f) * perimeter;

    // Avancement linéaire central (mètres)
    float delta_distance = (dist_l + dist_r) / 2.0f;

    // Mise à jour du Cap (Theta) avec le Gyroscope
    float delta_theta_deg = gyro_z_dps * delta_t_sec;
    float delta_theta_rad = delta_theta_deg * (PI / 180.0f);
    theta_rad += delta_theta_rad;

    // Normalisation (Garder Theta entre -PI et +PI)
    while (theta_rad > PI)  theta_rad -= 2.0f * PI;
    while (theta_rad <= -PI) theta_rad += 2.0f * PI;

    // Projection Trigonométrique X/Y
    pos_x += delta_distance * cos(theta_rad);
    pos_y += delta_distance * sin(theta_rad);

    // Sauvegarde pour la prochaine itération
    last_angle_l = current_angle_l;
    last_angle_r = current_angle_r;
}

float odometry_get_x() { return pos_x; }
float odometry_get_y() { return pos_y; }
float odometry_get_theta_rad() { return theta_rad; }
float odometry_get_theta_deg() { return theta_rad * (180.0f / PI); }