#include "AVOIDANCE.h"
#include "VL53L0X.h"   // NUM_TOF_SENSORS
#include <math.h>

// Défini dans main.cpp : -1 = capteur absent/invalide, 8190 = hors portée
extern volatile int tof_distances[NUM_TOF_SENSORS];

static AvoidMode _mode = AVOID_DEFAULT_MODE;

// Ordre de tof_distances[] (cf. interface web : ToF1 gauche, ToF2 centre, ToF3 droite)
// Angle relatif au cap du robot, positif = vers la gauche (anti-horaire)
static const uint8_t NUM_AVOID_SENSORS = 3;
static const float SENSOR_ANGLE_DEG[NUM_AVOID_SENSORS] = { +35.0f, 0.0f, -35.0f };
static const float SENSOR_GAIN[NUM_AVOID_SENSORS]      = { 0.5f,   1.0f, 0.5f };   // le central domine le vecteur
static const uint8_t CENTER_INDEX = 1;

void avoidance_set_mode(AvoidMode m) { _mode = m; }
AvoidMode avoidance_get_mode() { return _mode; }

bool avoidance_scan(float x, float y, float heading_deg, float threshold_mm, ObstacleInfo& out) {
    out = ObstacleInfo();

    float rep_x = 0, rep_y = 0, lateral = 0;
    float sum_x = 0, sum_y = 0;
    float min_d = 1e9f;
    bool center_hit = false;

    for (uint8_t i = 0; i < NUM_AVOID_SENSORS; i++) {
        int d = tof_distances[i];
        if (d < 0 || d >= (int)threshold_mm) continue;      // invalide ou rien sous le seuil

        float rel = SENSOR_ANGLE_DEG[i] * DEG_TO_RAD;
        float a = heading_deg * DEG_TO_RAD + rel;
        float ux = cosf(a), uy = sinf(a);
        float px = x + d * ux;
        float py = y + d * uy;

        // Impact sur un bord de table = mur, on l'ignore
        if (px < WALL_MARGIN_MM || px > TABLE_W_MM - WALL_MARGIN_MM ||
            py < WALL_MARGIN_MM || py > TABLE_L_MM - WALL_MARGIN_MM) continue;

        // Poids : 0 au seuil, 1 au contact. Chaque capteur pousse à l'opposé de sa direction.
        if (i == CENTER_INDEX) center_hit = true;

        float w = SENSOR_GAIN[i] * (threshold_mm - (float)d) / threshold_mm;
        rep_x   -= w * ux;
        rep_y   -= w * uy;
        lateral -= w * sinf(rel);

        out.px[out.n_points] = px;
        out.py[out.n_points] = py;
        out.n_points++;
        sum_x += px; sum_y += py;
        if (d < min_d) min_d = (float)d;
    }

    if (out.n_points == 0) return false;   // rien de valide : out reste vide

    out.cx = sum_x / out.n_points;
    out.cy = sum_y / out.n_points;
    out.min_dist = min_d;
    out.lateral = lateral;

    float n = sqrtf(rep_x * rep_x + rep_y * rep_y);
    if (n > 1e-3f) { out.rep_x = rep_x / n; out.rep_y = rep_y / n; }
    return center_hit;
}
