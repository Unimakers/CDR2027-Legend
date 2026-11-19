#include "TFT_LCD.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include "pamiboard.h"
#include "MCP23008.h"

// On indique -1 pour la broche RST car elle n'est pas sur l'ESP32 directement
Adafruit_ST7735 tft = Adafruit_ST7735(PAMI_CS, PAMI_DC, -1);

void screen_init() {
    // Initialisation du bus SPI
    SPI.begin(PAMI_SCLK, PAMI_MISO, PAMI_MOSI, PAMI_CS);
    
    // Reset matériel via le MCP23008
    mcp_reset_screen(); 
    delay(50); // Laisse l'écran s'allumer

    // Initialisation du pilote ST7735
    // INITR_BLACKTAB est le profil standard pour les 1.8" 128x160 (ST7735S)
    tft.initR(INITR_BLACKTAB); // ou INITR_GREENTAB, INITR_REDTAB
    
    // Orientation : 1 ou 3 = Paysage (Landscape), 0 ou 2 = Portrait
    tft.setRotation(3); 
    
    // Nettoyage de l'écran
    tft.fillScreen(SCREEN_BLACK);
}

void screen_clear() {
    tft.fillScreen(SCREEN_BLACK);
}

void screen_print(int x, int y, const char* text, uint16_t color, uint8_t size) {
    tft.setCursor(x, y);
    tft.setTextColor(color);
    tft.setTextSize(size);
    tft.print(text);
}

void screen_draw_dashboard(float voltage, float pos_x, float pos_y, const char* status) {
    
    // Titre (Fixe)
    screen_print(5, 5, "PAMI UNIMAKERS", SCREEN_YELLOW, 1);
    tft.drawLine(0, 15, 160, 15, SCREEN_WHITE);

    // Batterie
    tft.fillRect(60, 25, 60, 10, SCREEN_BLACK); // Efface l'ancienne valeur
    screen_print(5, 25, "BAT:", SCREEN_WHITE, 1);
    
    char bat_str[10];
    sprintf(bat_str, "%.1f V", voltage);
    uint16_t bat_color = (voltage > 11.0) ? SCREEN_GREEN : SCREEN_RED;
    screen_print(60, 25, bat_str, bat_color, 1);

    // Odométrie X / Y
    tft.fillRect(60, 45, 100, 20, SCREEN_BLACK); 
    screen_print(5, 45, "POS X:", SCREEN_WHITE, 1);
    screen_print(5, 55, "POS Y:", SCREEN_WHITE, 1);
    
    char x_str[10], y_str[10];
    sprintf(x_str, "%.2f", pos_x);
    sprintf(y_str, "%.2f", pos_y);
    screen_print(60, 45, x_str, SCREEN_WHITE, 1);
    screen_print(60, 55, y_str, SCREEN_WHITE, 1);

    // Statut
    tft.fillRect(5, 100, 150, 20, SCREEN_BLACK);
    screen_print(5, 100, status, SCREEN_BLUE, 2);
}