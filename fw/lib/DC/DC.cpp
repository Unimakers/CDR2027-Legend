#include "DC.h"
#include "pamiboard.h"

void dcmotors_init() {
    // Configuration des broches en sortie
    pinMode(PAMI_M1_IN1, OUTPUT);
    pinMode(PAMI_M1_IN2, OUTPUT);
    pinMode(PAMI_M2_IN1, OUTPUT);
    pinMode(PAMI_M2_IN2, OUTPUT);

    // Arrêt par défaut au démarrage
    dcmotors_stop_all();
}

void dcmotors_set_speed_m1(int speed) {
    // Sécurité : on bride la valeur entre -255 et 255
    if (speed > 255) speed = 255;
    if (speed < -255) speed = -255;

    if (speed > 0) {
        // Avance
        analogWrite(PAMI_M1_IN1, speed);
        analogWrite(PAMI_M1_IN2, 0);
    } 
    else if (speed < 0) {
        // Recule
        analogWrite(PAMI_M1_IN1, 0);
        analogWrite(PAMI_M1_IN2, -speed);
    } 
    else {
        // Stop
        analogWrite(PAMI_M1_IN1, 0);
        analogWrite(PAMI_M1_IN2, 0);
    }
}

void dcmotors_set_speed_m2(int speed) {
    if (speed > 255) speed = 255;
    if (speed < -255) speed = -255;

    if (speed > 0) {
        analogWrite(PAMI_M2_IN1, speed);
        analogWrite(PAMI_M2_IN2, 0);
    } 
    else if (speed < 0) {
        analogWrite(PAMI_M2_IN1, 0);
        analogWrite(PAMI_M2_IN2, -speed);
    } 
    else {
        analogWrite(PAMI_M2_IN1, 0);
        analogWrite(PAMI_M2_IN2, 0);
    }
}

void dcmotors_stop_all() {
    analogWrite(PAMI_M1_IN1, 0);
    analogWrite(PAMI_M1_IN2, 0);
    analogWrite(PAMI_M2_IN1, 0);
    analogWrite(PAMI_M2_IN2, 0);
}