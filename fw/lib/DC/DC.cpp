#include "DC.h"
#include "pamiboard.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include "soc/gpio_periph.h"
#include "soc/io_mux_reg.h"

// ---------------------------------------------------------------------------
// Mode de generation du PWM
//  - commente  : analogWrite() (fourni par ESP32Servo, comportement actuel)
//  - decommente: LEDC explicite, 20 kHz, canaux 4 a 7 (core Arduino ESP32 2.x)
// ---------------------------------------------------------------------------
// #define DC_USE_EXPLICIT_LEDC

#ifdef DC_USE_EXPLICIT_LEDC
constexpr int DC_PWM_FREQ = 20000;
constexpr int DC_PWM_RES  = 8;
constexpr int CH_M1_IN1 = 4, CH_M1_IN2 = 5, CH_M2_IN1 = 6, CH_M2_IN2 = 7;

static int channel_of(uint8_t pin) {
    if (pin == PAMI_M1_IN1) return CH_M1_IN1;
    if (pin == PAMI_M1_IN2) return CH_M1_IN2;
    if (pin == PAMI_M2_IN1) return CH_M2_IN1;
    return CH_M2_IN2;
}
static inline void pwm_write(uint8_t pin, int value) { ledcWrite(channel_of(pin), value); }
#else
static inline void pwm_write(uint8_t pin, int value) { analogWrite(pin, value); }
#endif

void dcmotors_init() {
#ifdef DC_USE_EXPLICIT_LEDC
    const uint8_t pins[4] = {PAMI_M1_IN1, PAMI_M1_IN2, PAMI_M2_IN1, PAMI_M2_IN2};
    for (int i = 0; i < 4; i++) {
        int ch = channel_of(pins[i]);
        ledcSetup(ch, DC_PWM_FREQ, DC_PWM_RES);
        ledcAttachPin(pins[i], ch);
    }
    Serial.println("[DC] Mode LEDC explicite 20 kHz (canaux 4-7)");
#else
    pinMode(PAMI_M1_IN1, OUTPUT);
    pinMode(PAMI_M1_IN2, OUTPUT);
    pinMode(PAMI_M2_IN1, OUTPUT);
    pinMode(PAMI_M2_IN2, OUTPUT);
    Serial.println("[DC] Mode analogWrite (ESP32Servo)");
#endif

    // Arret par defaut au demarrage
    dcmotors_stop_all();
}

void dcmotors_set_speed_m1(int speed) {
    if (speed > 255) speed = 255;
    if (speed < -255) speed = -255;

    if (speed > 0) {
        pwm_write(PAMI_M1_IN1, speed);
        pwm_write(PAMI_M1_IN2, 0);
    }
    else if (speed < 0) {
        pwm_write(PAMI_M1_IN1, 0);
        pwm_write(PAMI_M1_IN2, -speed);
    }
    else {
        pwm_write(PAMI_M1_IN1, 0);
        pwm_write(PAMI_M1_IN2, 0);
    }
}

void dcmotors_set_speed_m2(int speed) {
    if (speed > 255) speed = 255;
    if (speed < -255) speed = -255;

    if (speed > 0) {
        pwm_write(PAMI_M2_IN1, speed);
        pwm_write(PAMI_M2_IN2, 0);
    }
    else if (speed < 0) {
        pwm_write(PAMI_M2_IN1, 0);
        pwm_write(PAMI_M2_IN2, -speed);
    }
    else {
        pwm_write(PAMI_M2_IN1, 0);
        pwm_write(PAMI_M2_IN2, 0);
    }
}

void dcmotors_stop_all() {
    pwm_write(PAMI_M1_IN1, 0);
    pwm_write(PAMI_M1_IN2, 0);
    pwm_write(PAMI_M2_IN1, 0);
    pwm_write(PAMI_M2_IN2, 0);
}

// ===========================================================================
// SELF-TEST PWM : le signal est relu sur la broche, aucun moteur necessaire
// ===========================================================================

struct PinMeasure {
    float freq_hz;
    float duty_pct;
};

// Echantillonne la broche pendant window_us (frequence = fronts montants, duty = % d'echantillons a 1)
static PinMeasure measure_pin(uint8_t pin, uint32_t window_us) {
    // Active le tampon d'entree SANS toucher a la liaison PWM -> OUTPUT
    PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[pin]);

    uint32_t total = 0, high = 0, edges = 0;
    int last = gpio_get_level((gpio_num_t)pin);
    int64_t t0 = esp_timer_get_time();
    while ((uint32_t)(esp_timer_get_time() - t0) < window_us) {
        int v = gpio_get_level((gpio_num_t)pin);
        total++;
        high += v;
        if (v && !last) edges++;
        last = v;
    }

    PinMeasure m;
    m.freq_hz  = edges * 1000000.0f / (float)window_us;
    m.duty_pct = total ? (100.0f * (float)high / (float)total) : 0.0f;
    return m;
}

static const uint8_t TEST_PINS[4]    = {PAMI_M1_IN1, PAMI_M1_IN2, PAMI_M2_IN1, PAMI_M2_IN2};
static const char*   TEST_NAMES[4]   = {"M1_IN1", "M1_IN2", "M2_IN1", "M2_IN2"};
static const uint32_t WINDOW_US      = 200000; // 200 ms

// Mesure les 4 broches et compare a l'attendu (duty en %, -1 = pas de test)
static bool check_pins(const char* label, const float expected_duty[4], bool check_freq_consistency) {
    Serial.printf("[PWMTEST] --- %s ---\n", label);
    bool all_ok = true;
    float fmin = 1e9f, fmax = 0.0f;

    for (int i = 0; i < 4; i++) {
        PinMeasure m = measure_pin(TEST_PINS[i], WINDOW_US);
        bool ok = true;
        const char* why = "";

        if (expected_duty[i] == 0.0f) {
            if (m.duty_pct > 5.0f) { ok = false; why = "devrait etre a 0 %"; }
        } else if (expected_duty[i] > 0.0f) {
            if (m.duty_pct < expected_duty[i] - 12.0f || m.duty_pct > expected_duty[i] + 12.0f) {
                ok = false; why = "rapport cyclique incorrect (broche non pilotee en PWM ?)";
            } else if (m.freq_hz < 200.0f && m.duty_pct < 95.0f) {
                ok = false; why = "frequence trop basse (timer des servos 50 Hz ?)";
            }
            if (m.freq_hz > 0.0f) {
                if (m.freq_hz < fmin) fmin = m.freq_hz;
                if (m.freq_hz > fmax) fmax = m.freq_hz;
            }
        }

        Serial.printf("[PWMTEST] %s (GPIO%2d) : %7.0f Hz | duty %5.1f %% | attendu %s -> %s %s\n",
                      TEST_NAMES[i], TEST_PINS[i], m.freq_hz, m.duty_pct,
                      expected_duty[i] < 0 ? "-" : (expected_duty[i] == 0 ? "0 %" : "~50 %"),
                      ok ? "OK" : "PROBLEME", why);
        if (!ok) all_ok = false;
    }

    if (check_freq_consistency && fmax > 0.0f && fmax / fmin > 1.2f) {
        Serial.printf("[PWMTEST] ATTENTION : frequences differentes entre broches (%.0f a %.0f Hz) -> conflit de timer LEDC\n",
                      fmin, fmax);
        all_ok = false;
    }
    return all_ok;
}

bool dcmotors_pwm_selftest(const char* title) {
    Serial.println();
    Serial.printf("[PWMTEST] ========== SELF-TEST PWM DC %s ==========\n", title);
    bool global_ok = true;

    // 1) PWM 50 % sur chaque broche, une a la fois
    for (int i = 0; i < 4; i++) {
        dcmotors_stop_all();
        pwm_write(TEST_PINS[i], 128);
        float expected[4] = {0, 0, 0, 0};
        expected[i] = 50.0f;
        char label[48];
        snprintf(label, sizeof(label), "PWM 128 sur %s seul", TEST_NAMES[i]);
        if (!check_pins(label, expected, false)) global_ok = false;
    }

    // 2) Les 4 broches en PWM en meme temps : les frequences doivent etre identiques
    dcmotors_stop_all();
    for (int i = 0; i < 4; i++) pwm_write(TEST_PINS[i], 128);
    {
        float expected[4] = {50.0f, 50.0f, 50.0f, 50.0f};
        if (!check_pins("PWM 128 sur les 4 broches", expected, true)) global_ok = false;
    }

    // 3) Cas reels : M1 en marche arriere, M2 en avant
    dcmotors_stop_all();
    dcmotors_set_speed_m1(-128);
    dcmotors_set_speed_m2(128);
    {
        float expected[4] = {0.0f, 50.0f, 50.0f, 0.0f};
        if (!check_pins("M1=-128 (recule), M2=+128 (avance)", expected, false)) global_ok = false;
    }

    // 4) Inverse : M1 en avant, M2 en marche arriere
    dcmotors_stop_all();
    dcmotors_set_speed_m1(128);
    dcmotors_set_speed_m2(-128);
    {
        float expected[4] = {50.0f, 0.0f, 0.0f, 50.0f};
        if (!check_pins("M1=+128 (avance), M2=-128 (recule)", expected, false)) global_ok = false;
    }

    dcmotors_stop_all();
    Serial.printf("[PWMTEST] ========== RESULTAT GLOBAL : %s ==========\n\n",
                  global_ok ? "OK (PWM correct sur les 4 broches)" : "PROBLEME DETECTE (voir lignes ci-dessus)");
    return global_ok;
}
