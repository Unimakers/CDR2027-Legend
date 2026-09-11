#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>
#include <SPI.h>

// Configuration globale
#include "pamiboard.h"

// Composants
#include "MCP23008.h"
#include "AS5600.h"
#include "MPU9250.h"
#include "ODOMETRY.h"
#include "NEMA.h" 
#include "DC.h"
#include "SERVO.h"
#include "VL53L0X.h"
#include "BATTERY.h"
#include "TFT_LCD.h"
#include "NEOPIXEL.h"
#include "LUCKFOX.h"
#include "RFM69HCW.h"
#include "WEB_INTERFACE.h"
#include "TIMER.h"

// Tout en haut du fichier principal (hors des fonctions)
extern AsyncWebSocket ws;

// Objets Capteurs
AS5600Encoder encodeurG;
AS5600Encoder encodeurD;

// Handles des tâches FreeRTOS
TaskHandle_t TaskControlHandle;
TaskHandle_t TaskUIHandle;

// Minuteurs pour les tâches
PrecisionTimer matchTimer;        
PrecisionTimer loopTimer(20.0f);  

void IRAM_ATTR matchTimeoutCallback() {
}

// Variables pour le temps réel
unsigned long last_time_micros = 0;
int cpuLoadC1 = 0;

// Tableau de stockage des distances ToF pour faire le pont entre les 2 cœurs
int tof_distances[NUM_TOF_SENSORS] = {-1, -1, -1, -1}; 

// TÂCHE CORE 1 : CONTRÔLE ET ODOMÉTRIE (Haute Priorité)
void TaskControl(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(20); 

    for (;;) {
        loopTimer.start();

        // Lecture matérielle pure (I2C protégé sur ce cœur)
        encodeurG.update();
        encodeurD.update();
        mpu_update();
        battery_update(); 

        // Lecture I2C des ToF
        for (int i = 0; i < NUM_TOF_SENSORS; i++) {
            uint16_t dist = tof_get_distance(i);
            if (dist == 0) {
                tof_distances[i] = -1; // Capteur déconnecté / Erreur I2C
            } else {
                tof_distances[i] = dist; // Garde la valeur (y compris 8190 pour hors de portée)
            }
        }


        unsigned long current_time = micros();
        float delta_t_sec = (current_time - last_time_micros) / 1000000.0f;
        last_time_micros = current_time;

        odometry_update(
            encodeurG.get_angle_degrees(), 
            encodeurD.get_angle_degrees(), 
            mpu_get_gyro_z(), 
            delta_t_sec
        );

        cpuLoadC1 = (int)loopTimer.stopCpuLoad();

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

// TÂCHE CORE 0 : UI, RÉSEAU ET COMMUNICATIONS (Basse Priorité)
void TaskUI(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(100); 

    for (;;) {
        screen_draw_dashboard(
            battery_get_voltage(), 
            odometry_get_x(), 
            odometry_get_y(), 
            "RUNNING"
        );

        // --- Ajout des 4 valeurs ToF ---
        web_send_telemetry(
            odometry_get_x(), odometry_get_y(),
            encodeurG.get_total_ticks(), encodeurD.get_total_ticks(),
            encodeurG.get_angle_degrees(), encodeurD.get_angle_degrees(),
            battery_get_voltage(), mpu_get_gyro_z(), cpuLoadC1,
            tof_distances[0], tof_distances[1], tof_distances[2], tof_distances[3]
        );

        if (battery_get_voltage() > 0 && battery_get_voltage() < 11.0f) {
            neopixel_set_color_all(255, 0, 0);
        }

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

// CONTROLE COMMANDES WEB 
void executer_commande_web(String composant, int id, String valeur) {
    Serial.printf("[WEB] Composant : %s | ID: %d | Valeur: %s\n", composant.c_str(), id, valeur.c_str());

    if (composant == "servo") {
        if (id == 0) { servos_set_angle(0, valeur.toInt()); }
        if (id == 1) { servos_set_angle(1, valeur.toInt()); }
    } 
    else if (composant == "dc") {
        if (id == 1) dcmotors_set_speed_m1(valeur.toInt());
        if (id == 2) dcmotors_set_speed_m2(valeur.toInt());
    } 
    else if (composant == "neopixel") {
        if (valeur.startsWith("#") && valeur.length() == 7) {
            long number = strtol(&valeur[1], NULL, 16);
            uint8_t r = (number >> 16) & 0xFF;
            uint8_t g = (number >> 8) & 0xFF;
            uint8_t b = number & 0xFF;
            
            if (r == 0 && g == 0 && b == 0) {
                neopixel_clear();
            } else {
                neopixel_set_color_all(r, g, b);
            }
        } 
        else neopixel_clear();
    }
    else if (composant == "nema") {
        if (id == 1) nema_move(1, valeur.toInt()); 
        if (id == 2) nema_move(2, valeur.toInt()); 
        if (id == 3) nema_move(3, valeur.toInt()); 
    }
    else if (composant == "luckfox") {
        luckfox_send(valeur);
    }
}

void setup() {
    Serial.begin(115200);

    Wire.begin(PAMI_SDA1, PAMI_SCL1, 400000);
    Wire1.begin(PAMI_SDA2, PAMI_SCL2, 400000);

    if (mcp_init()) mcp_setup_board(); 
    
    screen_init();
    neopixel_init();
    battery_init();
    dcmotors_init();
    nema_init();
    servos_init();
    tof_init(); 

    mcp_enable_motors(true); 
    nema_set_microstepping(false, false); 
    nema_set_profile(1, 4000, 500); 
    nema_set_profile(2, 4000, 500);  

    encodeurG.init(&Wire);
    encodeurD.init(&Wire1);
    if (mpu_init()) mpu_calibrate();
    odometry_init(0.0f, 0.0f, 0.0f);

    luckfox_init(115200);
    web_init("PAMI Unimakers", "unimakers");
    web_set_command_handler(executer_commande_web);

    last_time_micros = micros();

    xTaskCreatePinnedToCore(
        TaskControl, "TaskControl", 8192, NULL, 3, &TaskControlHandle, 1 
    );

    xTaskCreatePinnedToCore(
        TaskUI, "TaskUI", 8192, NULL, 1, &TaskUIHandle, 0 
    );
}

void loop() {
    web_cleanup(); 

    String luckfox_msg;
    if (luckfox_receive(luckfox_msg)) {
        if (luckfox_msg.startsWith("<FILE_LIST_RESP:")) {
            int startIndex = 16;
            int endIndex = luckfox_msg.indexOf('>');
            if (endIndex != -1) {
                String files = luckfox_msg.substring(startIndex, endIndex);
                String jsonMsg = "{\"type\":\"file_list\",\"files\":\"" + files + "\"}";
                ws.textAll(jsonMsg);
                Serial.println("[ESP32] Liste des fichiers transmise au web : " + files);
            }
        }
        else {
            Serial.println("[LUCKFOX] : " + luckfox_msg);
        }
    }

    delay(10);
}