#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>
#include <SPI.h>

#include "pamiboard.h"

#include "MCP23008.h"
#include "AS5600.h"
#include "MPU9250.h"
#include "MOTION.h"
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
#include "OTA.h"
#include "STRATEGY.h"
#include "TIMER.h"
#include "AVOIDANCE.h"

extern AsyncWebSocket ws;

AS5600Encoder encodeurG;
AS5600Encoder encodeurD;

TaskHandle_t TaskControlHandle;
TaskHandle_t TaskToFHandle;
TaskHandle_t TaskUIHandle;

SemaphoreHandle_t mutex_Wire;
SemaphoreHandle_t mutex_Wire1;

PrecisionTimer matchTimer;
PrecisionTimer loopTimer(20.0f);

void IRAM_ATTR matchTimeoutCallback() {}

unsigned long last_time_micros = 0;
int cpuLoadC1 = 0;

// Lu par AVOIDANCE.cpp (extern) : 0 = gauche, 1 = centre, 2 = droite, 3 = autre
volatile int tof_distances[NUM_TOF_SENSORS] = {-1, -1, -1, -1};

void TaskControl(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(20);

    for (;;) {
        // OTA en cours : robot a l'arret, on laisse le CPU a l'ecriture en flash
        if (ota_is_active()) {
            vTaskDelay(pdMS_TO_TICKS(100));
            xLastWakeTime = xTaskGetTickCount();
            continue;
        }

        loopTimer.start();

        if (xSemaphoreTake(mutex_Wire, pdMS_TO_TICKS(15)) == pdTRUE) {
            encodeurG.update();
            mpu_update();
            xSemaphoreGive(mutex_Wire);
        }

        if (xSemaphoreTake(mutex_Wire1, pdMS_TO_TICKS(15)) == pdTRUE) {
            encodeurD.update();
            xSemaphoreGive(mutex_Wire1);
        }

        unsigned long current_time = micros();
        last_time_micros = current_time;

        long stepsL = nema_get_position(2);
        long stepsR = nema_get_position(1);

        motion_update_odometry(stepsL, stepsR, mpu_get_angle_z());

        strategy_update();

        cpuLoadC1 = (int)loopTimer.stopCpuLoad();

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void TaskToF(void *pvParameters) {
    for (;;) {
        if (ota_is_active()) {
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        if (xSemaphoreTake(mutex_Wire, pdMS_TO_TICKS(15)) == pdTRUE) {
            int d = tof_get_distance(0);
            tof_distances[0] = (d == 0) ? -1 : d;
            xSemaphoreGive(mutex_Wire);
        }
        vTaskDelay(pdMS_TO_TICKS(5));

        if (xSemaphoreTake(mutex_Wire, pdMS_TO_TICKS(15)) == pdTRUE) {
            int d = tof_get_distance(1);
            tof_distances[1] = (d == 0) ? -1 : d;
            xSemaphoreGive(mutex_Wire);
        }
        vTaskDelay(pdMS_TO_TICKS(5));

        if (xSemaphoreTake(mutex_Wire1, pdMS_TO_TICKS(15)) == pdTRUE) {
            int d = tof_get_distance(2);
            tof_distances[2] = (d == 0) ? -1 : d;
            xSemaphoreGive(mutex_Wire1);
        }
        vTaskDelay(pdMS_TO_TICKS(5));

        if (xSemaphoreTake(mutex_Wire1, pdMS_TO_TICKS(15)) == pdTRUE) {
            int d = tof_get_distance(3);
            tof_distances[3] = (d == 0) ? -1 : d;
            xSemaphoreGive(mutex_Wire1);
        }

        vTaskDelay(pdMS_TO_TICKS(40));
    }
}

void TaskUI(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(100);
    bool ota_affiche = false;

    for (;;) {
        // OTA en cours : un seul affichage "OTA", puis ni ecran ni telemetrie pour ne pas affamer le coeur 0 (watchdog)
        if (ota_is_active()) {
            if (!ota_affiche) {
                screen_draw_dashboard(battery_get_voltage(), robot_x, robot_y, "OTA");
                ota_affiche = true;
            }
            vTaskDelay(pdMS_TO_TICKS(100));
            xLastWakeTime = xTaskGetTickCount();
            continue;
        }
        ota_affiche = false;

        battery_update();

        screen_draw_dashboard(battery_get_voltage(), robot_x, robot_y, "RUNNING");

        // Odometrie en mm, la carte web travaille en metres
        web_send_telemetry(
            robot_x / 1000.0f, robot_y / 1000.0f,
            encodeurG.get_total_ticks(), encodeurD.get_total_ticks(),
            encodeurG.get_angle_degrees(), encodeurD.get_angle_degrees(),
            battery_get_voltage(), mpu_get_gyro_z(), mpu_get_angle_z(), cpuLoadC1,
            tof_distances[0], tof_distances[1], tof_distances[2], tof_distances[3]
        );

        if (strategy_is_running()) {
            web_send_strategy_debug(
                strategy_get_state(), strategy_get_step_index(), strategy_get_step_count(),
                strategy_get_target_heading(), mpu_get_angle_z()
            );
        }

        if (battery_get_voltage() > 0 && battery_get_voltage() < 11.0f) {
            neopixel_set_color_all(255, 0, 0);
        }

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

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
            if (r == 0 && g == 0 && b == 0) neopixel_clear();
            else neopixel_set_color_all(r, g, b);
        }
        else neopixel_clear();
    }
    else if (composant == "nema") {
        int p1 = valeur.indexOf('|');
        int p2 = valeur.indexOf('|', p1 + 1);
        int p3 = valeur.indexOf('|', p2 + 1);

        if (p1 != -1 && p2 != -1 && p3 != -1) {
            String mode = valeur.substring(0, p1);

            if (mode == "STOP") {
                nema_stop(id, true);
                return;
            }

            int v = valeur.substring(p1 + 1, p2).toInt();
            int a = valeur.substring(p2 + 1, p3).toInt();
            long cible = valeur.substring(p3 + 1).toInt();

            if (v <= 0) v = 100;
            if (a <= 0) a = 100;

            nema_set_profile(id, v, a);

            if (mode == "POS") {
                nema_move_to(id, cible);
            } else if (mode == "CONT") {
                if (cible > 0) nema_run_forward(id);
                else nema_run_backward(id);
            }
        }
    }
    else if (composant == "avoid") {
        if (valeur == "OFF")       avoidance_set_mode(AvoidMode::OFF);
        else if (valeur == "STOP") avoidance_set_mode(AvoidMode::STOP);
        else                       avoidance_set_mode(AvoidMode::AVOID);
        Serial.printf("[AVOID] mode %s\n", valeur.c_str());
    }
    else if (composant == "luckfox") {
        luckfox_send(valeur);
    }
}

// Robot immobile pendant l'ecriture en flash
void arret_pour_ota() {
    strategy_stop();
    nema_halt(3);
    dcmotors_stop_all();
}

void setup() {
    Serial.begin(115200);

    mutex_Wire = xSemaphoreCreateMutex();
    mutex_Wire1 = xSemaphoreCreateMutex();

    Wire.begin(PAMI_SDA1, PAMI_SCL1, 400000);
    Wire.setTimeOut(20);

    Wire1.begin(PAMI_SDA2, PAMI_SCL2, 400000);
    Wire1.setTimeOut(20);

    if (mcp_init()) mcp_setup_board();

    screen_init();
    neopixel_init();
    battery_init();
    dcmotors_init();

    // Activation des drivers avant nema_init() pour que le self-test NEMA puisse faire tourner les moteurs
    if (battery_get_voltage() > 10.0f) {
        mcp_enable_motors(true);
    } else {
        Serial.printf("[NEMA] Batterie %.2fV < 10V : drivers desactives\n", battery_get_voltage());
    }

    nema_init();
    servos_init();

    // --- TEST PWM DC (a retirer une fois le probleme resolu) ---
    dcmotors_pwm_selftest("(apres servos_init)");
    servos_set_angle(0, 45);
    servos_set_angle(1, 135);
    delay(300);
    dcmotors_pwm_selftest("(servos en mouvement)");
    servos_set_angle(0, 90);
    servos_set_angle(1, 90);
    // ---------------------------------------------------------
    
    tof_init();

    if (!encodeurG.init(&Wire))  Serial.println("Erreur : encodeur gauche introuvable !");
    if (!encodeurD.init(&Wire1)) Serial.println("Erreur : encodeur droit introuvable !");

    if (mpu_init()) mpu_calibrate();

    motion_init(58.0f, 90.0f, 3200);
    motion_set_position(0.0f, 0.0f, 0.0f);

    luckfox_init(115200);
    web_init("PAMI Unimakers", "unimakers");
    strategy_init();
    web_set_command_handler(executer_commande_web);
    ota_init("pami");
    ota_set_start_handler(arret_pour_ota);

    last_time_micros = micros();

    xTaskCreatePinnedToCore(TaskControl, "TaskControl", 8192, NULL, 3, &TaskControlHandle, 1);
    xTaskCreatePinnedToCore(TaskToF, "TaskToF", 4096, NULL, 1, &TaskToFHandle, 0);
    xTaskCreatePinnedToCore(TaskUI, "TaskUI", 8192, NULL, 1, &TaskUIHandle, 0);
}

void loop() {
    ota_handle();
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
            }
        } else {
            Serial.println("[LUCKFOX] : " + luckfox_msg);
        }
    }

    delay(10);
}
