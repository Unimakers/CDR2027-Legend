#include "NEOPIXEL.h"
#include "pamiboard.h"
#include <Arduino.h>

// Réglage de la luminosité (0 à 100 %)
static uint8_t luminosite_pct = 40;
static uint32_t derniere_couleur = 0xFFFFFFFF;

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

static inline uint8_t appliquer_luminosite(uint8_t val) {
    return (uint8_t)((val * luminosite_pct) / 100);
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

// Émission d'une trame GRB complète (format natif WS2812B)
static void IRAM_ATTR send_color_frame(uint8_t r, uint8_t g, uint8_t b) {
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
        send_ws2812_byte(g, c_t0h, c_t1h, c_period); // Vert en premier (GRB standard)
        send_ws2812_byte(r, c_t0h, c_t1h, c_period); // Rouge
        send_ws2812_byte(b, c_t0h, c_t1h, c_period); // Bleu
    }

    portEXIT_CRITICAL(&mux);

    // Latch de repos au niveau bas (> 80 µs)
    *gpio_clr_reg = gpio_mask;
    delayMicroseconds(100);
}

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

    derniere_couleur = 0;
    send_color_frame(0, 0, 0);
}

void neopixel_clear() {
    if (derniere_couleur == 0) return;
    derniere_couleur = 0;
    send_color_frame(0, 0, 0);
}

void neopixel_set_color_all(uint8_t r, uint8_t g, uint8_t b) {
    uint8_t r_scaled = appliquer_luminosite(r);
    uint8_t g_scaled = appliquer_luminosite(g);
    uint8_t b_scaled = appliquer_luminosite(b);

    uint32_t color = ((uint32_t)r_scaled << 16) | ((uint32_t)g_scaled << 8) | b_scaled;
    if (color == derniere_couleur) return;
    derniere_couleur = color;

    send_color_frame(r_scaled, g_scaled, b_scaled);
}

void neopixel_set_color_pixel(uint16_t index, uint8_t r, uint8_t g, uint8_t b) {
    neopixel_set_color_all(r, g, b);
}

void neopixel_set_brightness(uint8_t pct) {
    if (pct > 100) pct = 100;
    luminosite_pct = pct;
    derniere_couleur = 0xFFFFFFFF;
}