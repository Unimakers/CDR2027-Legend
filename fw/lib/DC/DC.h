#pragma once
#include <Arduino.h>

// Initialize DC motor pins
void dcmotors_init();

// Set Motor 1 speed (Range: -255 to 255)
// Positive = Forward, Negative = Backward, 0 = Stop
void dcmotors_set_speed_m1(int speed);

// Set Motor 2 speed (Range: -255 to 255)
void dcmotors_set_speed_m2(int speed);

// Immediately stop both motors (Coast / Freewheel)
void dcmotors_stop_all();