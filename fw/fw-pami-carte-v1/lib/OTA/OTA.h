#pragma once
#include <Arduino.h>
#include <functional>

using OtaStartHandler = std::function<void()>;

// A appeler apres web_init() : le WiFi doit etre lance
void ota_init(const char* hostname);

// Appele juste avant l'ecriture en flash (mise en securite du robot)
void ota_set_start_handler(OtaStartHandler handler);

// A appeler regulierement depuis loop()
void ota_handle();

bool ota_is_active();
