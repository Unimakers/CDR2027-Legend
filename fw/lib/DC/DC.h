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

// Diagnostic : relit les broches IN1/IN2 des 2 ponts et affiche dans le Serial
// la frequence et le rapport cyclique reels (moteur non necessaire).
// A appeler APRES servos_init(). Retourne true si tout est OK.
bool dcmotors_pwm_selftest(const char* title = "");
