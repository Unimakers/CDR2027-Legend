#include "MPU9250.h"
#include <Wire.h>

constexpr uint8_t MPU_ADDR = 0x68;
constexpr float GYRO_SCALE_FACTOR = 131.0f; 

static float gyro_z_offset = 0.0f;
static float current_gyro_z = 0.0f;

// --- NOUVELLES VARIABLES ---
static float absolute_angle_z = 0.0f;
static unsigned long last_mpu_time = 0;

// Detection de capteur fige : le bruit du gyro fait toujours varier la mesure brute,
// meme a l'arret. Des valeurs strictement identiques = MPU reinitialise/en veille.
static const int FROZEN_SAMPLES_LIMIT = 8;  // 160 ms a 50 Hz
static int16_t last_raw_z = 0;
static int same_raw_count = 0;
static float suspect_angle = 0.0f;          // angle integre depuis que la mesure se repete

static bool mpu_write_reg(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(reg);
    Wire.write(value);
    return Wire.endTransmission() == 0;
}

// Configuration complete : a refaire apres toute reinitialisation du capteur
static bool mpu_configure() {
    bool ok = mpu_write_reg(0x6B, 0x01);  // PWR_MGMT_1 : reveil, horloge PLL gyro
    ok &= mpu_write_reg(0x1A, 0x03);      // CONFIG : filtre passe-bas gyro 41 Hz (vibrations moteurs)
    ok &= mpu_write_reg(0x1B, 0x00);      // GYRO_CONFIG : +-250 deg/s (131 LSB/deg/s)
    return ok;
}

bool mpu_init() {
    bool ok = mpu_configure();
    delay(50);

    last_mpu_time = micros(); // Initialisation du chronomètre
    return ok;
}

void mpu_calibrate() {
    Serial.println("MPU Calibration... DO NOT MOVE THE ROBOT!");
    // On attend 2 secondes pleines pour que les vibrations mécaniques s'estompent
    delay(2000); 
    
    long sum_z = 0;
    const int num_samples = 2000; // 2000 échantillons au lieu de 500 pour lisser le bruit
    
    for (int i = 0; i < num_samples; i++) {
        Wire.beginTransmission(MPU_ADDR);
        Wire.write(0x47); 
        Wire.endTransmission();
        
        Wire.requestFrom((uint8_t)MPU_ADDR, (uint8_t)2);
        if (Wire.available() >= 2) {
            int16_t raw_z = (Wire.read() << 8) | Wire.read();
            sum_z += raw_z;
        }
        delay(2);
    }
    
    gyro_z_offset = (float)sum_z / num_samples;
    Serial.printf("MPU Gyro Z Offset: %.2f\n", gyro_z_offset);
    
    absolute_angle_z = 0.0f;      
    last_mpu_time = micros();     
}

void mpu_update() {
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x47);

    if (Wire.endTransmission() == 0) {
        Wire.requestFrom((uint8_t)MPU_ADDR, (uint8_t)2);

        if (Wire.available() >= 2) {
            int16_t raw_z = (Wire.read() << 8) | Wire.read();

            same_raw_count = (raw_z == last_raw_z) ? same_raw_count + 1 : 0;
            last_raw_z = raw_z;
            if (same_raw_count == 0) suspect_angle = 0.0f;
            if (same_raw_count >= FROZEN_SAMPLES_LIMIT) {
                // On annule ce qui a ete integre avec la valeur figee, puis on reveille le capteur
                Serial.printf("[MPU] Lecture figee (raw %d), reconfiguration du capteur\n", raw_z);
                absolute_angle_z -= suspect_angle;
                suspect_angle = 0.0f;
                mpu_configure();
                same_raw_count = 0;
                current_gyro_z = 0.0f;
                last_mpu_time = micros();
                return;
            }

            float corrected_z = (float)raw_z - gyro_z_offset;
            current_gyro_z = corrected_z / GYRO_SCALE_FACTOR;
            
            // Zone mort
            if (abs(current_gyro_z) < 0.4f) {
                current_gyro_z = 0.0f;
            }
            
            // Intégration
            unsigned long current_time = micros();
            float dt = (current_time - last_mpu_time) / 1000000.0f; 
            last_mpu_time = current_time;
            
            absolute_angle_z += (current_gyro_z * dt);
            if (same_raw_count > 0) suspect_angle += current_gyro_z * dt;
        }
    }
}

float mpu_get_gyro_z() {
    return current_gyro_z;
}

float mpu_get_angle_z() {
    return absolute_angle_z;
}

void mpu_reset_angle() {
    absolute_angle_z = 0.0f;
}

void mpu_set_angle(float deg) {
    absolute_angle_z = deg;
}