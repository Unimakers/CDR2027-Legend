#include "MPU9250.h"
#include <Wire.h>

constexpr uint8_t MPU_ADDR = 0x68;
constexpr float GYRO_SCALE_FACTOR = 131.0f; 

static float gyro_z_offset = 0.0f;
static float current_gyro_z = 0.0f;

// --- NOUVELLES VARIABLES ---
static float absolute_angle_z = 0.0f;
static unsigned long last_mpu_time = 0;

bool mpu_init() {
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x6B); 
    Wire.write(0x00);    
    uint8_t error = Wire.endTransmission();
    
    last_mpu_time = micros(); // Initialisation du chronomètre
    return (error == 0);
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
            float corrected_z = (float)raw_z - gyro_z_offset;
            current_gyro_z = corrected_z / GYRO_SCALE_FACTOR;
            
            // Zone mort
            if (abs(current_gyro_z) < 0.02f) {
                current_gyro_z = 0.0f;
            }
            
            // Intégration
            unsigned long current_time = micros();
            float dt = (current_time - last_mpu_time) / 1000000.0f; 
            last_mpu_time = current_time;
            
            absolute_angle_z += (current_gyro_z * dt);
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