#include "NEOPIXEL.h"
#include "pamiboard.h"
#include <Arduino.h>
#include <math.h>

// Réglage de la luminosité (0 à 100 %)
static volatile uint8_t luminosite_pct = 40;

// Rendu des animations
static const uint32_t PERIODE_IMAGE_MS = 25;   // ~40 images/s
static const uint32_t DUREE_FONDU_MS   = 350;  // fondu entre deux modes

// Consigne partagée entre les tâches appelantes et la tâche de rendu
static portMUX_TYPE consigne_mux = portMUX_INITIALIZER_UNLOCKED;
static NeoAnim consigne_anim = NeoAnim::FIXE;
static uint8_t consigne_rgb[3] = {0, 0, 0};
static uint8_t consigne_fixe[NUM_NEOPIXELS][3] = {};   // couleurs par LED du mode FIXE
static uint32_t consigne_version = 0;

static uint8_t trame_envoyee[NUM_NEOPIXELS][3];        // dernière trame émise (R, G, B)

// Pointeurs directs vers les registres matériels GPIO de l'ESP32-S3
static volatile uint32_t *gpio_set_reg = nullptr;
static volatile uint32_t *gpio_clr_reg = nullptr;
static uint32_t gpio_mask = 0;

// Lecture directe du registre de cycles processeur Xtensa (1 cycle = 4.16 ns à 240 MHz)
static inline uint32_t IRAM_ATTR get_cpu_cycles() {
    uint32_t ccount;
    asm volatile("rsr %0, ccount" : "=a"(ccount));
    return ccount;
}

// Émission d'un bit WS2812 avec timings calés sur l'horloge CPU
static inline void IRAM_ATTR send_ws2812_bit(bool bit, uint32_t cycles_high, uint32_t cycles_period) {
    uint32_t start = get_cpu_cycles();
    *gpio_set_reg = gpio_mask;
    while ((get_cpu_cycles() - start) < cycles_high) {}
    *gpio_clr_reg = gpio_mask;
    while ((get_cpu_cycles() - start) < cycles_period) {}
}

// Émission d'un octet (MSB en premier)
static void IRAM_ATTR send_ws2812_byte(uint8_t b, uint32_t c_t0h, uint32_t c_t1h, uint32_t c_period) {
    for (int i = 7; i >= 0; i--) {
        if (b & (1 << i)) {
            send_ws2812_bit(true, c_t1h, c_period);
        } else {
            send_ws2812_bit(false, c_t0h, c_period);
        }
    }
}

// Émission d'une trame GRB complète (format natif WS2812B), une couleur par LED
static void IRAM_ATTR send_frame(const uint8_t (*rgb)[3]) {
    if (!gpio_set_reg) return;

    // Calcul dynamique selon la fréquence CPU réelle (ex: 240 MHz)
    uint32_t mhz = getCpuFrequencyMhz();
    uint32_t c_t0h    = (350 * mhz) / 1000;  // ~350 ns HIGH
    uint32_t c_t1h    = (800 * mhz) / 1000;  // ~800 ns HIGH
    uint32_t c_period = (1250 * mhz) / 1000; // ~1250 ns période complète

    // Section critique ultra-brève (~30 µs par LED) : isole des interruptions système
    portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL(&mux);

    for (uint16_t n = 0; n < NUM_NEOPIXELS; n++) {
        send_ws2812_byte(rgb[n][1], c_t0h, c_t1h, c_period); // Vert en premier (GRB standard)
        send_ws2812_byte(rgb[n][0], c_t0h, c_t1h, c_period); // Rouge
        send_ws2812_byte(rgb[n][2], c_t0h, c_t1h, c_period); // Bleu
    }

    portEXIT_CRITICAL(&mux);

    // Latch de repos au niveau bas (> 80 µs)
    *gpio_clr_reg = gpio_mask;
    delayMicroseconds(100);
}

// ---------------- Animations ----------------
// Chaque animation calcule, pour un instant t (ms depuis son lancement), le niveau 0..1 de chaque LED.
// Uniquement des sinus et des fondus : aucune transition brutale.

static inline float borne01(float x) { return x < 0.0f ? 0.0f : (x > 1.0f ? 1.0f : x); }

// Fondu adouci (départ et arrivée progressifs)
static inline float adouci(float x) { x = borne01(x); return x * x * (3.0f - 2.0f * x); }

// Phase 0..1 d'un cycle de période donnée
static inline float phase(uint32_t t_ms, uint32_t periode_ms) {
    return (float)(t_ms % periode_ms) / (float)periode_ms;
}

// Bosse en cosinus (0 -> 1 -> 0) entre debut et debut + duree
static float bosse(float t_ms, float debut_ms, float duree_ms) {
    if (t_ms < debut_ms || t_ms > debut_ms + duree_ms) return 0.0f;
    return 0.5f - 0.5f * cosf(2.0f * PI * (t_ms - debut_ms) / duree_ms);
}

static float niveau_anim(NeoAnim anim, uint16_t i, uint32_t t_ms) {
    switch (anim) {
        case NeoAnim::RESPIRATION: {
            // Cycle de 3,2 s, jamais totalement éteint
            float s = 0.5f - 0.5f * cosf(2.0f * PI * phase(t_ms, 3200));
            return 0.12f + 0.88f * s;
        }
        case NeoAnim::BATTEMENT: {
            // Deux pulsations rapprochées (la seconde plus faible), puis repos : cycle de 1,8 s
            float t = (float)(t_ms % 1800);
            float p = fmaxf(bosse(t, 0.0f, 420.0f), 0.6f * bosse(t, 360.0f, 480.0f));
            return 0.08f + 0.92f * p;
        }
        case NeoAnim::BALAYAGE: {
            // Tache lumineuse qui va et vient en 2 s, ralentie aux extrémités
            float pos = (NUM_NEOPIXELS - 1) * (0.5f - 0.5f * cosf(2.0f * PI * phase(t_ms, 2000)));
            float d = (float)i - pos;
            return 0.05f + 0.95f * expf(-(d * d) / 2.9f);
        }
        case NeoAnim::VAGUE: {
            // Une longueur d'onde sur tout le bandeau (raccord propre si les LED forment un anneau)
            float s = 0.5f + 0.5f * sinf(2.0f * PI * ((float)i / NUM_NEOPIXELS - phase(t_ms, 2400)));
            return 0.15f + 0.85f * s;
        }
        case NeoAnim::CHARGEMENT: {
            // Remplissage LED par LED en 1,6 s, puis extinction en fondu sur 0,8 s
            float t = (float)(t_ms % 2400);
            float rempli = adouci(((t / 1600.0f) * (NUM_NEOPIXELS + 1)) - (float)i);
            float sortie = 1.0f - adouci((t - 1600.0f) / 800.0f);
            return 0.04f + 0.96f * rempli * sortie;
        }
        default:
            return 1.0f;
    }
}

// Teinte 0..1 -> RGB, légèrement désaturé pour rester doux
static void teinte_vers_rgb(float h, float rgb[3]) {
    const float saturation = 0.85f;
    const float decalage[3] = {5.0f, 3.0f, 1.0f};
    for (int c = 0; c < 3; c++) {
        float k = fmodf(decalage[c] + h * 6.0f, 6.0f);
        float m = borne01(fminf(k, 4.0f - k));
        rgb[c] = 255.0f * (1.0f - saturation * m);
    }
}

static void neopixel_task(void *pvParameters) {
    float depart[NUM_NEOPIXELS][3] = {};    // image affichée au moment du changement de mode
    float affiche[NUM_NEOPIXELS][3] = {};   // image courante (avant luminosité)
    uint8_t fixe[NUM_NEOPIXELS][3] = {};
    uint8_t base[3] = {0, 0, 0};
    NeoAnim anim = NeoAnim::FIXE;
    uint32_t version = 0;
    uint32_t t_debut = millis();

    for (;;) {
        portENTER_CRITICAL(&consigne_mux);
        bool change = (consigne_version != version);
        if (change) {
            version = consigne_version;
            anim = consigne_anim;
            memcpy(base, consigne_rgb, sizeof(base));
            memcpy(fixe, consigne_fixe, sizeof(fixe));
        }
        portEXIT_CRITICAL(&consigne_mux);

        uint32_t maintenant = millis();
        if (change) {
            memcpy(depart, affiche, sizeof(depart));
            t_debut = maintenant;
        }
        uint32_t t = maintenant - t_debut;
        float fondu = adouci((float)t / DUREE_FONDU_MS);
        uint8_t lum = luminosite_pct;

        uint8_t trame[NUM_NEOPIXELS][3];
        for (uint16_t i = 0; i < NUM_NEOPIXELS; i++) {
            float cible[3];
            if (anim == NeoAnim::FIXE) {
                for (int c = 0; c < 3; c++) cible[c] = fixe[i][c];
            } else if (anim == NeoAnim::ARC_EN_CIEL) {
                // Un tour complet de teintes réparti sur le bandeau, qui défile en 6 s
                teinte_vers_rgb(phase(t, 6000) + (float)i / NUM_NEOPIXELS, cible);
            } else {
                // Niveau au carré : la progression paraît linéaire à l'oeil
                float n = niveau_anim(anim, i, t);
                n *= n;
                for (int c = 0; c < 3; c++) cible[c] = base[c] * n;
            }

            for (int c = 0; c < 3; c++) {
                affiche[i][c] = depart[i][c] + (cible[c] - depart[i][c]) * fondu;
                trame[i][c] = (uint8_t)(affiche[i][c] * lum / 100.0f + 0.5f);
            }
        }

        // Rien à émettre tant que l'image ne change pas (couleur fixe, bandeau éteint)
        if (memcmp(trame, trame_envoyee, sizeof(trame)) != 0) {
            memcpy(trame_envoyee, trame, sizeof(trame));
            send_frame(trame);
        }

        vTaskDelay(pdMS_TO_TICKS(PERIODE_IMAGE_MS));
    }
}

// ---------------- API ----------------

void neopixel_init() {
    pinMode(PAMI_NEOPIXEL, OUTPUT);
    digitalWrite(PAMI_NEOPIXEL, LOW);

    // Attribution dynamique des registres selon le GPIO (évite le warning de décalage 32 bits)
    if (PAMI_NEOPIXEL < 32) {
        gpio_set_reg = (volatile uint32_t *)&GPIO.out_w1ts;
        gpio_clr_reg = (volatile uint32_t *)&GPIO.out_w1tc;
        gpio_mask    = (1UL << (PAMI_NEOPIXEL & 31));
    } else {
        gpio_set_reg = (volatile uint32_t *)&GPIO.out1_w1ts.val;
        gpio_clr_reg = (volatile uint32_t *)&GPIO.out1_w1tc.val;
        gpio_mask    = (1UL << ((PAMI_NEOPIXEL - 32) & 31));
    }

    memset(trame_envoyee, 0, sizeof(trame_envoyee));
    send_frame(trame_envoyee);

    // Coeur 0 : la section critique d'émission ne retarde pas l'asservissement (coeur 1)
    xTaskCreatePinnedToCore(neopixel_task, "TaskNeopixel", 4096, NULL, 2, NULL, 0);
}

void neopixel_clear() {
    neopixel_set_color_all(0, 0, 0);
}

void neopixel_set_color_all(uint8_t r, uint8_t g, uint8_t b) {
    portENTER_CRITICAL(&consigne_mux);
    bool identique = (consigne_anim == NeoAnim::FIXE);
    for (uint16_t i = 0; i < NUM_NEOPIXELS && identique; i++) {
        identique = (consigne_fixe[i][0] == r && consigne_fixe[i][1] == g && consigne_fixe[i][2] == b);
    }
    if (!identique) {
        consigne_anim = NeoAnim::FIXE;
        for (uint16_t i = 0; i < NUM_NEOPIXELS; i++) {
            consigne_fixe[i][0] = r; consigne_fixe[i][1] = g; consigne_fixe[i][2] = b;
        }
        consigne_version++;
    }
    portEXIT_CRITICAL(&consigne_mux);
}

void neopixel_set_color_pixel(uint16_t index, uint8_t r, uint8_t g, uint8_t b) {
    if (index >= NUM_NEOPIXELS) return;
    portENTER_CRITICAL(&consigne_mux);
    consigne_anim = NeoAnim::FIXE;
    consigne_fixe[index][0] = r; consigne_fixe[index][1] = g; consigne_fixe[index][2] = b;
    consigne_version++;
    portEXIT_CRITICAL(&consigne_mux);
}

void neopixel_set_animation(NeoAnim anim, uint8_t r, uint8_t g, uint8_t b) {
    if (anim == NeoAnim::FIXE) { neopixel_set_color_all(r, g, b); return; }

    portENTER_CRITICAL(&consigne_mux);
    // Même animation déjà en cours : on ne la relance pas (appel possible à chaque tour de boucle)
    bool identique = (consigne_anim == anim && consigne_rgb[0] == r && consigne_rgb[1] == g && consigne_rgb[2] == b);
    if (!identique) {
        consigne_anim = anim;
        consigne_rgb[0] = r; consigne_rgb[1] = g; consigne_rgb[2] = b;
        consigne_version++;
    }
    portEXIT_CRITICAL(&consigne_mux);
}

void neopixel_set_brightness(uint8_t pct) {
    if (pct > 100) pct = 100;
    luminosite_pct = pct;
}
