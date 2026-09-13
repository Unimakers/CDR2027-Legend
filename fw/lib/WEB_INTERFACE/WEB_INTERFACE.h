#pragma once
#include <Arduino.h>
#include <functional> // Permet de passer des fonctions en paramètre

using WebCommandHandler = std::function<void(String composant, int id, String valeur)>;

bool web_init(const char* ap_ssid, const char* ap_password);

// Fonction pour lier le serveur web au main.cpp
void web_set_command_handler(WebCommandHandler handler);

void web_send_telemetry(float x, float y, long enc_l, long enc_r, float angle_l, float angle_r, float battery, float gyro_z, float gyro_angle, int cpu_load, int tof1, int tof2, int tof3, int tof4);
void web_cleanup();