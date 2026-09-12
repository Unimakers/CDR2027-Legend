#include "BATTERY.h"
#include "pamiboard.h"

// Formule du pont diviseur inversée
constexpr float VOLTAGE_MULTIPLIER = ((R1 + R2) / R2) * correctionFactor;

// Variable statique pour garder la tension en mémoire
static float current_voltage = 0.0f;

void battery_init() {
    pinMode(PAMI_VCC_CALC, INPUT);
    
    // première lecture au démarrage
    battery_update(); 
}

void battery_update() {
    // Suréchantillonnage (Moyenne sur 64 lectures)
    uint32_t sum_mv = 0;
    const int NUM_SAMPLES = 64;
    
    for (int i = 0; i < NUM_SAMPLES; i++) {
        sum_mv += analogReadMilliVolts(PAMI_VCC_CALC);
    }
    
    float avg_mv = (float)sum_mv / NUM_SAMPLES;

    // Conversion en Volts dynamique 
    // multiplie la valeur lue par ratio calculé automatiquement
    float raw_voltage = (avg_mv / 1000.0f) * VOLTAGE_MULTIPLIER;

    // Noise Gate (Tension Fantôme)
    if (raw_voltage < 5.0f) {
        current_voltage = 0.0f;
    } else {
        current_voltage = raw_voltage;
    }
}

float battery_get_voltage() {
    return current_voltage;
}