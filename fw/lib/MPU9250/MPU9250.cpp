#include "MPU9250.h"
#include <Wire.h>

// Paramètres internes
constexpr uint8_t MPU_ADDR = 0x68;
constexpr float GYRO_SCALE_FACTOR = 131.0f; // Pour +/- 250 deg/s

// Variables privées au fichier
static float gyro_z_offset = 0.0f;
static float current_gyro_z = 0.0f;

bool mpu_init() {
    // Réveil du MPU9250 (Registre PWR_MGMT_1)
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x6B); 
    Wire.write(0x00);    
    uint8_t error = Wire.endTransmission();
    
    return (error == 0);
}

void mpu_calibrate() {
    Serial.println("MPU Calibration... DO NOT MOVE THE ROBOT!");
    delay(500); // Stabilisation
    
    long sum_z = 0;
    const int num_samples = 500;
    
    for (int i = 0; i < num_samples; i++) {
        Wire.beginTransmission(MPU_ADDR);
        Wire.write(0x47); // Registre GYRO_ZOUT_H
        Wire.endTransmission();
        
        Wire.requestFrom((uint8_t)MPU_ADDR, (uint8_t)2);
        if (Wire.available() >= 2) {
            int16_t raw_z = (Wire.read() << 8) | Wire.read();
            sum_z += raw_z;
        }
        delay(3);
    }
    
    gyro_z_offset = (float)sum_z / num_samples;
    Serial.printf("MPU Gyro Z Offset: %.2f\n", gyro_z_offset);
}

void mpu_update() {
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x47); // Registre GYRO_ZOUT_H
    
    if (Wire.endTransmission() == 0) {
        Wire.requestFrom((uint8_t)MPU_ADDR, (uint8_t)2);
        
        if (Wire.available() >= 2) {
            int16_t raw_z = (Wire.read() << 8) | Wire.read();
            
            // Application de la correction et conversion en degrés/seconde
            float corrected_z = (float)raw_z - gyro_z_offset;
            current_gyro_z = corrected_z / GYRO_SCALE_FACTOR;
        }
    }
}

float mpu_get_gyro_z() {
    return current_gyro_z;
}