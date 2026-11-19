#pragma once
#include <Arduino.h>

// Initialize the MCP23008 on the I2C bus
bool mcp_init();

// Configure all pins according to pamiboard.h specifics
void mcp_setup_board();

// Basic GPIO functions for the MCP23008
void mcp_pinMode(uint8_t pin, uint8_t mode);
void mcp_write(uint8_t pin, uint8_t value);
uint8_t mcp_read(uint8_t pin);

// Helpers (Optional but very useful)
void mcp_enable_motors(bool state); // Controls TMC2209 EN pin
void mcp_reset_screen();            // Sends a pulse to the screen RST