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
TaskHandle_t TaskToFHandle;
TaskHandle_t TaskUIHandle;

// Mutex pour protéger les bus I2C
SemaphoreHandle_t mutex_Wire;
SemaphoreHandle_t mutex_Wire1;

// Minuteurs pour les tâches
PrecisionTimer matchTimer;        
PrecisionTimer loopTimer(20.0f);  

void IRAM_ATTR matchTimeoutCallback() {
}

// Variables pour le temps réel
unsigned long last_time_micros = 0;
int cpuLoadC1 = 0;

// Tableau "volatile" pour forcer la synchronisation entre les cœurs
volatile int tof_distances[NUM_TOF_SENSORS] = {-1, -1, -1, -1}; 

// TÂCHE CORE 1 : CONTRÔLE ET ODOMÉTRIE (Boucle : 20 ms / Priorité : 3)
void TaskControl(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(20); 

    for (;;) {
        loopTimer.start();
        loopTimer.start();

        // 1. Lecture Encodeur Gauche & MPU (Sur Wire / I2C1)
        if (xSemaphoreTake(mutex_Wire, pdMS_TO_TICKS(15)) == pdTRUE) {
            encodeurG.update();
            mpu_update();
            xSemaphoreGive(mutex_Wire);
        }

        // 2. Lecture Encodeur Droit (Sur Wire1 / I2C2)
        if (xSemaphoreTake(mutex_Wire1, pdMS_TO_TICKS(15)) == pdTRUE) {
            encodeurD.update();
            xSemaphoreGive(mutex_Wire1);
        }

        // 1. Lecture Encodeur Gauche & MPU (Sur Wire / I2C1)
        if (xSemaphoreTake(mutex_Wire, pdMS_TO_TICKS(5)) == pdTRUE) {
            encodeurG.update();
            mpu_update();
            xSemaphoreGive(mutex_Wire);
        }

        // 2. Lecture Encodeur Droit (Sur Wire1 / I2C2)
        if (xSemaphoreTake(mutex_Wire1, pdMS_TO_TICKS(5)) == pdTRUE) {
            encodeurD.update();
            xSemaphoreGive(mutex_Wire1);
        }

        unsigned long current_time = micros();
        float delta_t_sec = (current_time - last_time_micros) / 1000000.0f;
        last_time_micros = current_time;

        // Mise à jour de l'odométrie
        odometry_update(
            encodeurG.get_angle_degrees(), 
            encodeurD.get_angle_degrees(), 
            mpu_get_gyro_z(), 
            delta_t_sec
        );

        // --- LA RÉGULATION ET L'ÉVITEMENT SERONT À CODER ICI ---
        // Ex: if (tof_distances[1] > 0 && tof_distances[1] < 300) { obstacle() }

        cpuLoadC1 = (int)loopTimer.stopCpuLoad();

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

// TÂCHE CORE 0 : GESTION DES ToF (Boucle relâchée / Priorité : 1)
void TaskToF(void *pvParameters) {
    for (;;) {
        // --- ToF 0 (I2C1) ---
        if (xSemaphoreTake(mutex_Wire, pdMS_TO_TICKS(15)) == pdTRUE) {
            int d = tof_get_distance(0);
            tof_distances[0] = (d == 0) ? -1 : d;
            xSemaphoreGive(mutex_Wire);
        }
        vTaskDelay(pdMS_TO_TICKS(5)); // Respiration pour le Wi-Fi et TaskControl

        // --- ToF 1 (I2C1) ---
        if (xSemaphoreTake(mutex_Wire, pdMS_TO_TICKS(15)) == pdTRUE) {
            int d = tof_get_distance(1);
            tof_distances[1] = (d == 0) ? -1 : d;
            xSemaphoreGive(mutex_Wire);
        }
        vTaskDelay(pdMS_TO_TICKS(5));

        // --- ToF 2 (I2C2) ---
        if (xSemaphoreTake(mutex_Wire1, pdMS_TO_TICKS(15)) == pdTRUE) {
            int d = tof_get_distance(2);
            tof_distances[2] = (d == 0) ? -1 : d;
            xSemaphoreGive(mutex_Wire1);
        }
        vTaskDelay(pdMS_TO_TICKS(5));

        // --- ToF 3 (I2C2) ---
        if (xSemaphoreTake(mutex_Wire1, pdMS_TO_TICKS(15)) == pdTRUE) {
            int d = tof_get_distance(3);
            tof_distances[3] = (d == 0) ? -1 : d;
            xSemaphoreGive(mutex_Wire1);
        }

        // Pause de fin de cycle
        vTaskDelay(pdMS_TO_TICKS(40));
    }
}

// TÂCHE CORE 0 : UI, RÉSEAU ET COMMUNICATIONS (Boucle : 100 ms / Priorité : 1)
void TaskUI(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(100); 

    for (;;) {
        // Déplacé ici car lent et peu critique
        battery_update(); 

        screen_draw_dashboard(
            battery_get_voltage(), 
            odometry_get_x(), 
            odometry_get_y(), 
            "RUNNING"
        );

        web_send_telemetry(
            odometry_get_x(), odometry_get_y(),
            encodeurG.get_total_ticks(), 
            encodeurD.get_total_ticks(),
            encodeurG.get_angle_degrees(), 
            encodeurD.get_angle_degrees(),
            battery_get_voltage(), 
            mpu_get_gyro_z(),
            mpu_get_angle_z(), 
            cpuLoadC1,
            tof_distances[0], 
            tof_distances[1], 
            tof_distances[2], 
            tof_distances[3]
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
        int p1 = valeur.indexOf('|');
        int p2 = valeur.indexOf('|', p1 + 1);
        int p3 = valeur.indexOf('|', p2 + 1);
        
        if (p1 != -1 && p2 != -1 && p3 != -1) {
            String mode = valeur.substring(0, p1);
            int v = valeur.substring(p1 + 1, p2).toInt();
            int a = valeur.substring(p2 + 1, p3).toInt();
            long cible = valeur.substring(p3 + 1).toInt();
            
            nema_set_profile(id, v, a);
            
            if (mode == "POS") {
                nema_move(id, cible); 
            } 
            else if (mode == "CONT") {
                if (cible > 0) {
                    nema_run_forward(id);
                } else {
                    nema_run_backward(id);
                }
            } 
            else if (mode == "STOP") {
                nema_stop(id, true); 
            }
        }
    }
    else if (composant == "luckfox") {
        luckfox_send(valeur);
    }
}

void setup() {
    Serial.begin(115200);

    // Initialisation des Mutex avant toute utilisation
    mutex_Wire = xSemaphoreCreateMutex();
    mutex_Wire1 = xSemaphoreCreateMutex();

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

    if (battery_get_voltage() > 10.0f) {
        mcp_enable_motors(true); 
    }

    encodeurG.init(&Wire);
    encodeurD.init(&Wire1);
    if (mpu_init()) mpu_calibrate();
    odometry_init(0.0f, 0.0f, 0.0f);

    luckfox_init(115200);
    web_init("PAMI Unimakers", "unimakers");
    web_set_command_handler(executer_commande_web);

    last_time_micros = micros();

    // Tâche critique : Régulation (Core 1)
    xTaskCreatePinnedToCore(TaskControl, "TaskControl", 8192, NULL, 3, &TaskControlHandle, 1);

    // Tâche moyenne : Capteurs ToF (Core 0, Prio 2)
    xTaskCreatePinnedToCore(TaskToF, "TaskToF", 4096, NULL, 1, &TaskToFHandle, 0);

    // Tâche lente : Affichages et Web (Core 0, Prio 1)
    xTaskCreatePinnedToCore(TaskUI, "TaskUI", 8192, NULL, 1, &TaskUIHandle, 0);
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