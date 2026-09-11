#pragma once
#include <Arduino.h>

// UART x2 //
constexpr uint8_t PAMI_UART0_RX = 44;
constexpr uint8_t PAMI_UART1_RX = 18;
constexpr uint8_t PAMI_UART1_TX = 8;

// SPI x1 //
constexpr uint8_t PAMI_SCLK = 35;
constexpr uint8_t PAMI_MOSI = 36;
constexpr uint8_t PAMI_DC = 37;
constexpr uint8_t PAMI_MISO = 38;
constexpr uint8_t PAMI_CS = 39; // SCREEN CONTROL PIN
constexpr uint8_t PAMI_CS2 = 43; // RFM MODULE CONTROL PIN
constexpr uint8_t PAMI_IRQ = 40; // INTERRUPT PIN (RFM MODULE)

// I2C x2 //
constexpr uint8_t PAMI_SCL1 = 1;
constexpr uint8_t PAMI_SDA1 = 2;
constexpr uint8_t PAMI_SCL2 = 42;
constexpr uint8_t PAMI_SDA2 = 41;

// NEMA x3 //
constexpr uint8_t PAMI_DIR1 = 14;
constexpr uint8_t PAMI_STEP1 = 13;
constexpr uint8_t PAMI_DIR2 = 17;
constexpr uint8_t PAMI_STEP2 = 16;
constexpr uint8_t PAMI_DIR3 = 12;
constexpr uint8_t PAMI_STEP3 = 11;
constexpr uint8_t PAMI_MS1 = 47; // MICRO STEP CONFIG
constexpr uint8_t PAMI_MS2 = 21; // MICRO STEP CONFIG

// DC MOTORS x2 //
constexpr uint8_t PAMI_M1_IN1 = 15;
constexpr uint8_t PAMI_M1_IN2 = 7;
constexpr uint8_t PAMI_M2_IN1 = 6;
constexpr uint8_t PAMI_M2_IN2 = 5;

// SERVOS x2 //
constexpr uint8_t PAMI_SERVO0 = 9;
constexpr uint8_t PAMI_SERVO1 = 10;

// NEOPIXEL //
constexpr uint8_t PAMI_NEOPIXEL = 48;
constexpr uint8_t NUM_NEOPIXELS = 10; // Nombre de LED NeoPixel 

// GPIOS EXTENDER FROM MCP23008 //
constexpr uint8_t PAMI_MCP_RST = 0; // RST SCREEN
constexpr uint8_t PAMI_MCP_TEAM_BTN = 1;
constexpr uint8_t PAMI_MCP_LAUNCHPULL = 2;
constexpr uint8_t PAMI_MCP_EN = 3; // EN PIN TMC2209
constexpr uint8_t PAMI_MCP_XSHUT3 = 4; // TOF OFF PIN
constexpr uint8_t PAMI_MCP_XSHUT2 = 5; // TOF OFF PIN
constexpr uint8_t PAMI_MCP_XSHUT1 = 6; // TOF OFF PIN
constexpr uint8_t PAMI_MCP_XSHUT0 = 7; // TOF OFF PIN

// VOLTAGE CALCULATOR 0 to 28,4V +-50mV //
constexpr uint8_t PAMI_VCC_CALC = 4;
constexpr float R1 = 200000.0f; // ACCORDING TO THE VALUE (R) ON PCB
constexpr float R2 = 33000.0f; // ACCORDING TO THE VALUE (R) ON PCB
constexpr float correctionFactor = 1.00f; // FACTOR TO CORRECT THE VOLTAGE CALCULATION
