#pragma once
#include <Arduino.h>

// Wake up the MPU9250 on the I2C bus
bool mpu_init();

// Calibrate the Z-axis gyroscope (Robot must remain strictly still!)
void mpu_calibrate();

// Read the sensor and update internal values (Call this in your main loop)
void mpu_update();

// Get the latest Z-axis rotation speed in degrees per second (°/s)
float mpu_get_gyro_z();