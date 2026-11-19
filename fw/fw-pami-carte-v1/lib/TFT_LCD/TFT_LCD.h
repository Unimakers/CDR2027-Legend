#pragma once
#include <Arduino.h>
#include <stdint.h> // Pour les types uint16_t

// Initialise le bus SPI et l'écran TFT
void screen_init();

// Efface l'écran (Noir par défaut)
void screen_clear();

// Affiche un texte simple à des coordonnées précises
void screen_print(int x, int y, const char* text, uint16_t color = 0xFFFF, uint8_t size = 1);

// Affiche un tableau de bord pré-formaté avec les données du robot
void screen_draw_dashboard(float voltage, float pos_x, float pos_y, const char* status);

// --- Couleurs de base (Format RGB565) ---
constexpr uint16_t SCREEN_BLACK   = 0x0000;
constexpr uint16_t SCREEN_WHITE   = 0xFFFF;
constexpr uint16_t SCREEN_RED     = 0xF800;
constexpr uint16_t SCREEN_GREEN   = 0x07E0;
constexpr uint16_t SCREEN_BLUE    = 0x001F;
constexpr uint16_t SCREEN_YELLOW  = 0xFFE0;