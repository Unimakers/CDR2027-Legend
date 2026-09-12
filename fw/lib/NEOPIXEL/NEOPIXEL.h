#pragma once
#include <Arduino.h>

// Initialize the NeoPixel strip
void neopixel_init();

// Turn off all LEDs
void neopixel_clear();

// Set all LEDs to the exact same color (RGB from 0 to 255)
void neopixel_set_color_all(uint8_t r, uint8_t g, uint8_t b);

// Set a specific LED to a specific color (index starts at 0)
void neopixel_set_color_pixel(uint16_t index, uint8_t r, uint8_t g, uint8_t b);