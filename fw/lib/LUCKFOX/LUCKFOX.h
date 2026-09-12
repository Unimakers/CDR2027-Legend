#pragma once
#include <Arduino.h>

/*
ssh root@172.32.0.93
luckfox
on same IP Window 
*/

// Initialise le port série 1 (UART1) vers le Luckfox
// Le baudrate standard est 115200, mais tu peux le monter à 921600 si tu envoies beaucoup de data
void luckfox_init(uint32_t baudrate = 115200);

// Envoie une commande ou une requête (ex: "GET_PATH", "CAMERA_ON")
void luckfox_send(const String& message);

// Vérifie si une réponse complète (terminée par \n) est arrivée.
// Renvoie 'true' et remplit la variable 'response' si un message est prêt.
bool luckfox_receive(String& response);