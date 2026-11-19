#include "NEMA.h"
#include <FastAccelStepper.h>
#include "pamiboard.h"

FastAccelStepperEngine engine = FastAccelStepperEngine();

FastAccelStepper *stepper1 = nullptr;
FastAccelStepper *stepper2 = nullptr;
FastAccelStepper *stepper3 = nullptr;

// Diagnostic : impulsions STEP generees a la main, sans FastAccelStepper.
// Un moteur qui ne bouge pas pendant ce test a un probleme materiel (piste, soudure, driver).
// Decommenter pour relancer le test (les roues tournent au demarrage : robot sur cales).
// #define NEMA_GPIO_SELFTEST

#ifdef NEMA_GPIO_SELFTEST
static void nema_gpio_selftest() {
    const uint8_t step_pins[3] = {PAMI_STEP1, PAMI_STEP2, PAMI_STEP3};
    const uint8_t dir_pins[3]  = {PAMI_DIR1, PAMI_DIR2, PAMI_DIR3};
    for (int m = 0; m < 3; m++) {
        pinMode(step_pins[m], OUTPUT);
        pinMode(dir_pins[m], OUTPUT);
        digitalWrite(dir_pins[m], HIGH);
        Serial.printf("[NEMA] Self-test moteur %d (STEP GPIO%d, DIR GPIO%d) : 1/4 de tour\n",
                      m + 1, step_pins[m], dir_pins[m]);
        for (int i = 0; i < 800; i++) {
            digitalWrite(step_pins[m], HIGH); delayMicroseconds(10);
            digitalWrite(step_pins[m], LOW);  delayMicroseconds(1240);
        }
        delay(500);
    }
}
#endif

// Driver RMT : les pas sont comptes en logiciel.
// Le driver MCPWM/PCNT relit la broche STEP par l'IOMUX (Arduino core 2.x = IDF 4.4) :
// sur l'ESP32-S3 ca ne comptait que sur GPIO16 (moteur 2), les moteurs 1 et 3 restaient bloques.
static const FasDriver NEMA_DRIVER = DRIVER_RMT;

static FastAccelStepper* connect_stepper(uint8_t step_pin, uint8_t dir_pin) {
    FastAccelStepper* s = engine.stepperConnectToPin(step_pin, NEMA_DRIVER);
    if (s) {
        s->setDirectionPin(dir_pin);
        s->setAutoEnable(false);
    }
    return s;
}

static void log_stepper(uint8_t id, FastAccelStepper* s) {
    if (s) Serial.printf("[NEMA] Moteur %d connecte, driver %s\n", id, s->driverTypeString());
    else   Serial.printf("[NEMA] Moteur %d NON connecte\n", id);
}

void nema_init() {
    pinMode(PAMI_MS1, OUTPUT);
    pinMode(PAMI_MS2, OUTPUT);
    nema_set_microstepping(HIGH, HIGH);

#ifdef NEMA_GPIO_SELFTEST
    nema_gpio_selftest();
#endif

    engine.init();

    stepper1 = connect_stepper(PAMI_STEP1, PAMI_DIR1);
    stepper2 = connect_stepper(PAMI_STEP2, PAMI_DIR2);
    stepper3 = connect_stepper(PAMI_STEP3, PAMI_DIR3);

    log_stepper(1, stepper1);
    log_stepper(2, stepper2);
    log_stepper(3, stepper3);

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

void nema_move(uint8_t motor_id, long steps) {
    FastAccelStepper* s = get_stepper(motor_id);
    if (s) s->move(steps);
}

void nema_set_profile(uint8_t motor_id, uint32_t speed_hz, uint32_t accel) {
    FastAccelStepper* s = get_stepper(motor_id);
    if (s) {
        s->setSpeedInHz(speed_hz);
        s->setAcceleration(accel);
    } else {
        Serial.printf("[ERREUR] Moteur %d non initialise !\n", motor_id);
    }
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

// Arret immediat sans rampe, en conservant la position (odometrie intacte)
void nema_halt(uint8_t motor_id) {
    FastAccelStepper* s = get_stepper(motor_id);
    if (s) s->forceStop();
}

long nema_get_position(uint8_t motor_id) {
    FastAccelStepper* s = get_stepper(motor_id);
    return s ? s->getCurrentPosition() : 0;
}

void nema_move_to(uint8_t motor_id, long absolute_position) {
    FastAccelStepper* s = get_stepper(motor_id);
    if (s) s->moveTo(absolute_position);
}

bool nema_is_running(uint8_t motor_id) {
    FastAccelStepper* s = get_stepper(motor_id);
    return s ? s->isRunning() : false;
}