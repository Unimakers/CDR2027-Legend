#include "VL53L0X.h"
#include <Adafruit_VL53L0X.h>
#include "pamiboard.h"
#include "MCP23008.h" 
#include <Wire.h> // Nécessaire pour accéder à &Wire et &Wire1

Adafruit_VL53L0X tofs[NUM_TOF_SENSORS];
const uint8_t TOF_ADDRESSES[NUM_TOF_SENSORS] = { 0x30, 0x31, 0x32, 0x33 };
const uint8_t TOF_XSHUT_PINS[NUM_TOF_SENSORS] = {
    PAMI_MCP_XSHUT0, PAMI_MCP_XSHUT1, PAMI_MCP_XSHUT2, PAMI_MCP_XSHUT3
};

// Tableau pour mémoriser le statut d'initialisation de chaque capteur
bool tof_active[NUM_TOF_SENSORS] = {false, false, false, false};

bool tof_init() {
    bool all_ok = true;

    // Reset matériel de tous les capteurs
    for (int i = 0; i < NUM_TOF_SENSORS; i++) {
        mcp_write(TOF_XSHUT_PINS[i], LOW);
    }
    delay(10); 

    // Initialisation individuelle
    for (int i = 0; i < NUM_TOF_SENSORS; i++) {
        mcp_write(TOF_XSHUT_PINS[i], HIGH);
        delay(10); 

        // Sélection dynamique du bus : I2C1 (Wire) pour 0 et 1, I2C2 (Wire1) pour 2 et 3
        TwoWire* wireBus = (i < 2) ? &Wire : &Wire1;

        // begin(adresse, debug, pointeur_bus_i2c)
        if (!tofs[i].begin(TOF_ADDRESSES[i], false, wireBus)) {
            Serial.printf("Erreur : ToF %d introuvable sur son bus !\n", i);
            tof_active[i] = false;
            all_ok = false;
            continue; 
        }
        
        tofs[i].configSensor(Adafruit_VL53L0X::VL53L0X_SENSE_HIGH_SPEED);
        tof_active[i] = true;
    }

    if (all_ok) {
        Serial.println("Tous les ToF sont initialises avec succes (I2C1 & I2C2).");
    } else {
        Serial.println("Attention : Un ou plusieurs ToF sont manquants.");
    }
    return all_ok;
}

uint16_t tof_get_distance(uint8_t sensor_index) {
    if (sensor_index >= NUM_TOF_SENSORS) return 0; 
    
    // Blocage de la requête si le capteur n'a pas été initialisé
    if (!tof_active[sensor_index]) return 0; 

    VL53L0X_RangingMeasurementData_t measure;
    tofs[sensor_index].rangingTest(&measure, false); 

    if (measure.RangeStatus != 4) {  
        return measure.RangeMilliMeter;
    } else {
        return 8190; 
    }
}