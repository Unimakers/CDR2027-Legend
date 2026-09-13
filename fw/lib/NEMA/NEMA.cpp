#include "NEMA.h"
#include <FastAccelStepper.h>
#include "pamiboard.h"

FastAccelStepperEngine engine = FastAccelStepperEngine();

FastAccelStepper *stepper1 = nullptr;
FastAccelStepper *stepper2 = nullptr;
FastAccelStepper *stepper3 = nullptr;

void nema_init() {
    pinMode(PAMI_MS1, OUTPUT);
    pinMode(PAMI_MS2, OUTPUT);
    nema_set_microstepping(HIGH, HIGH); 

    engine.init();

    // Attachement Moteur 1
    #if defined(SUPPORT_ESP32_RMT)
    stepper1 = engine.stepperConnectToPin(PAMI_STEP1, DRIVER_RMT);
    #else
    stepper1 = engine.stepperConnectToPin(PAMI_STEP1);
    #endif
    if (stepper1) {
        stepper1->setDirectionPin(PAMI_DIR1);
        stepper1->setAutoEnable(false); 
    }

    // Attachement Moteur 2
    #if defined(SUPPORT_ESP32_RMT)
    stepper2 = engine.stepperConnectToPin(PAMI_STEP2, DRIVER_RMT);
    #else
    stepper2 = engine.stepperConnectToPin(PAMI_STEP2);
    #endif
    if (stepper2) {
        stepper2->setDirectionPin(PAMI_DIR2);
        stepper2->setAutoEnable(false);
    }

    // Attachement Moteur 3
    #if defined(SUPPORT_ESP32_RMT)
    stepper3 = engine.stepperConnectToPin(PAMI_STEP3, DRIVER_RMT);
    #else
    stepper3 = engine.stepperConnectToPin(PAMI_STEP3);
    #endif
    if (stepper3) {
        stepper3->setDirectionPin(PAMI_DIR3);
        stepper3->setAutoEnable(false);
    }

    // Les profils de vitesse et d'accélération sont définis pour chaque moteur
    nema_set_profile(1, 4000, 500); 
    nema_set_profile(2, 4000, 500);  
    nema_set_profile(3, 4000, 500);  
}

void nema_set_microstepping(bool ms1_high, bool ms2_high) {
    digitalWrite(PAMI_MS1, ms1_high ? HIGH : LOW);
    digitalWrite(PAMI_MS2, ms2_high ? HIGH : LOW);
}

FastAccelStepper* get_stepper(uint8_t motor_id) {
    if (motor_id == 1) return stepper1;
    if (motor_id == 2) return stepper2;
    if (motor_id == 3) return stepper3;
    return nullptr;
}

void nema_set_profile(uint8_t motor_id, uint32_t speed_hz, uint32_t accel) {
    FastAccelStepper* s = get_stepper(motor_id);
    if (s) {
        // Enregistre les valeurs
        s->setSpeedInHz(speed_hz);
        s->setAcceleration(accel);
        
        // Force l'application immédiate dans le timer matériel de l'ESP32
        s->applySpeedAcceleration(); 
    } else {
        Serial.printf("[ERREUR] Moteur %d non initialise !\n", motor_id);
    }
}

void nema_move(uint8_t motor_id, long steps) {
    FastAccelStepper* s = get_stepper(motor_id);
    if (s) s->move(steps);
}

void nema_run_forward(uint8_t motor_id) {
    FastAccelStepper* s = get_stepper(motor_id);
    if (s) s->runForward();
}

void nema_run_backward(uint8_t motor_id) {
    FastAccelStepper* s = get_stepper(motor_id);
    if (s) s->runBackward();
}

void nema_stop(uint8_t motor_id, bool force_stop) {
    FastAccelStepper* s = get_stepper(motor_id);
    if (s) {
        if (force_stop) {
            s->forceStopAndNewPosition(0);
        } else {
            s->stopMove();
        }
    }
}