#pragma once
#include <Arduino.h>

// Animations disponibles. Toutes sont non bloquantes : l'appel ne fait que changer
// de mode, le rendu est fait par une tache dediee (~40 images/s) avec un fondu entre deux modes.
enum class NeoAnim : uint8_t {
    FIXE = 0,       // Couleur fixe
    RESPIRATION,    // Fondu lent montant/descendant        -> robot pret, en attente
    BATTEMENT,      // Double pulsation douce puis pause    -> alerte (batterie faible)
    BALAYAGE,       // Point lumineux en va-et-vient        -> deplacement / strategie en cours
    VAGUE,          // Ondulation qui parcourt le bandeau   -> obstacle, robot en pause
    CHARGEMENT,     // Remplissage progressif puis fondu    -> initialisation, calibration
    ARC_EN_CIEL     // Degrade qui defile (couleur ignoree) -> fin de match
};
constexpr uint8_t NEO_ANIM_COUNT = 7;

// Initialize the NeoPixel strip (starts the animation task)
void neopixel_init();

// Turn off all LEDs
void neopixel_clear();

// Set all LEDs to the exact same color (RGB from 0 to 255)
void neopixel_set_color_all(uint8_t r, uint8_t g, uint8_t b);

// Set a specific LED to a specific color (index starts at 0)
void neopixel_set_color_pixel(uint16_t index, uint8_t r, uint8_t g, uint8_t b);

// Start an animation with a base color. Safe to call in a loop : calling it again
// with the same animation and color does not restart it.
void neopixel_set_animation(NeoAnim anim, uint8_t r, uint8_t g, uint8_t b);

// Global brightness (0 to 100 %)
void neopixel_set_brightness(uint8_t pct);
