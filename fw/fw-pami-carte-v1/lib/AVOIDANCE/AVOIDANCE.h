#pragma once
#include <Arduino.h>

// OFF   : aucune détection
// STOP  : arrêt simple si obstacle, reprise quand la voie est libre
// AVOID : évitement vectoriel (déplacement du point de passage / détour), sinon arrêt simple
enum class AvoidMode : uint8_t { OFF, STOP, AVOID };

// Mode au démarrage : changer ici (ou à chaud via la commande web "avoid")
constexpr AvoidMode AVOID_DEFAULT_MODE = AvoidMode::AVOID;

// Table (repère carte web, en mm). Un impact hors de la table (+ marge) est un mur, pas un obstacle.
constexpr float TABLE_W_MM     = 2000.0f;
constexpr float TABLE_L_MM     = 3000.0f;
constexpr float WALL_MARGIN_MM = 100.0f;   // marge > derive odometrique attendue

struct ObstacleInfo {
    uint8_t n_points = 0;          // nombre d'impacts valides
    float px[3] = {0, 0, 0};       // points d'impact dans le repère monde (mm)
    float py[3] = {0, 0, 0};
    float min_dist = 0;            // distance du plus proche impact (mm)
    float cx = 0, cy = 0;          // centroïde des impacts (mm)
    float rep_x = 0, rep_y = 0;    // vecteur répulsif unitaire (monde), s'éloigne de l'obstacle
    float lateral = 0;             // composante latérale du répulsif dans le repère robot : > 0 = fuir vers la gauche
};

void avoidance_set_mode(AvoidMode m);
AvoidMode avoidance_get_mode();

// Lit tof_distances[] (0 = gauche +35°, 1 = centre 0°, 2 = droite -35°).
// Renvoie true UNIQUEMENT si le ToF central (priorité absolue) voit un impact valide sous threshold_mm.
// Les ToF latéraux ne déclenchent jamais : leurs impacts (< threshold_mm) sont ajoutés à 'out'
// pour choisir le côté de contournement et vérifier qu'un point est libre.
// x, y en mm, heading_deg = cap du robot (0° = +X, anti-horaire).
bool avoidance_scan(float x, float y, float heading_deg, float threshold_mm, ObstacleInfo& out);
