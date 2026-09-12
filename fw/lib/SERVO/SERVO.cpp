#include "SERVO.h"
#include <ESP32Servo.h>
#include "pamiboard.h"

// Objets 
Servo servo_0;
Servo servo_1;

void servos_init() {
    // Allocation des timers matériels de l'ESP32 pour éviter les conflits
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);
    
    // Fréquence standard pour les servomoteurs (50 Hz)
    servo_0.setPeriodHertz(50);
    servo_1.setPeriodHertz(50);
    
    // Attachement avec les durées d'impulsion min/max
    servo_0.attach(PAMI_SERVO0, 500, 2400);
    servo_1.attach(PAMI_SERVO1, 500, 2400);
    
    // Position de sécurité au démarrage (Milieu)
    servos_set_angle(0, 90);
    servos_set_angle(1, 90);
}

void servos_set_angle(uint8_t servo_id, uint8_t angle) {
    // Forcer l'angle entre 0 et 180
    if (angle > 180) angle = 180;
    
    if (servo_id == 0) {
        servo_0.write(angle);
    } 
    else if (servo_id == 1) {
        servo_1.write(angle);
    }
}

void servos_detach(uint8_t servo_id) {
    if (servo_id == 0 && servo_0.attached()) {
        servo_0.detach();
    } 
    else if (servo_id == 1 && servo_1.attached()) {
        servo_1.detach();
    }
}