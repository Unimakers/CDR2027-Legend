#include "MCP23008.h"
#include <Adafruit_MCP23008.h>
#include "Wire.h"
#include "pamiboard.h"

// objet MCP
Adafruit_MCP23008 mcp;

bool mcp_init() {
    if (!mcp.begin(0x20, &Wire1)) {
        Serial.println("Erreur : MCP23008 non trouvé !");
        return false;
    }
    return true;
}

void mcp_setup_board() {
    // Inputs (Boutons avec Pull-up interne du MCP)
    mcp_pinMode(PAMI_MCP_TEAM_BTN, INPUT_PULLUP);
    mcp_pinMode(PAMI_MCP_LAUNCHPULL, INPUT_PULLUP);

    // Outputs (Reset écran & Enable Moteurs)
    mcp_pinMode(PAMI_MCP_RST, OUTPUT);
    mcp_pinMode(PAMI_MCP_EN, OUTPUT);

    // Outputs (Capteurs ToF - XSHUT)
    mcp_pinMode(PAMI_MCP_XSHUT0, OUTPUT);
    mcp_pinMode(PAMI_MCP_XSHUT1, OUTPUT);
    mcp_pinMode(PAMI_MCP_XSHUT2, OUTPUT);
    mcp_pinMode(PAMI_MCP_XSHUT3, OUTPUT);

    // États par défaut au démarrage
    mcp_write(PAMI_MCP_RST, HIGH); // L'écran n'est pas en reset
    mcp_enable_motors(false);      // Moteurs désactivés par sécurité
    
    // On éteint tous les ToF par défaut
    mcp_write(PAMI_MCP_XSHUT0, LOW);
    mcp_write(PAMI_MCP_XSHUT1, LOW);
    mcp_write(PAMI_MCP_XSHUT2, LOW);
    mcp_write(PAMI_MCP_XSHUT3, LOW);
}

void mcp_pinMode(uint8_t pin, uint8_t mode) {
    mcp.pinMode(pin, mode);
}

void mcp_write(uint8_t pin, uint8_t value) {
    mcp.digitalWrite(pin, value);
}

uint8_t mcp_read(uint8_t pin) {
    return mcp.digitalRead(pin);
}

void mcp_enable_motors(bool enable) {
    mcp_write(PAMI_MCP_EN, enable ? LOW : HIGH);
}

void mcp_reset_screen() {
    mcp_write(PAMI_MCP_RST, LOW);
    delay(50);
    mcp_write(PAMI_MCP_RST, HIGH);
    delay(50);
}