#include "WEB_INTERFACE.h"
#include <WiFi.h>
#include <LittleFS.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// Variable qui retient la fonction du main.cpp
static WebCommandHandler global_command_handler = nullptr;

void web_set_command_handler(WebCommandHandler handler) {
    global_command_handler = handler;
}

void onWsMessage(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    if (type == WS_EVT_DATA) {
        AwsFrameInfo *info = (AwsFrameInfo*)arg;
        if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
            data[len] = 0;
            String message = (char*)data;
            
            // On désérialise la commande JSON (ex: {"cmd":"servo", "id":0, "val":"90"})
            StaticJsonDocument<256> doc;
            DeserializationError error = deserializeJson(doc, message);
            
            if (!error && global_command_handler != nullptr) {
                // On prévient le main.cpp qu'un ordre est arrivé !
                String cmd = doc["cmd"] | "";
                int id = doc["id"] | 0;
                String val = doc["val"] | "";
                
                global_command_handler(cmd, id, val);
            }
        }
    }
}

bool web_init(const char* ap_ssid, const char* ap_password) {
    // Démarrage du Point d'Accès WiFi
    WiFi.softAP(ap_ssid, ap_password);
    Serial.print("Point d'acces WiFi lance. IP : ");
    Serial.println(WiFi.softAPIP());

    // Démarrage du système de fichiers
    if (!LittleFS.begin(true)) {
        Serial.println("Erreur Montage LittleFS");
        return false;
    }
    Serial.println("LittleFS OK.");

    // Configuration du WebSocket
    ws.onEvent(onWsMessage);
    server.addHandler(&ws);

    // Routage des fichiers statiques (HTML, CSS, JS)
    server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

    // 5. Démarrage du serveur web
    server.begin();
    return true;
}

void web_send_telemetry(float x, float y, long enc_l, long enc_r, float angle_l, float angle_r, float battery, float gyro_z, int cpu_load, int tof1, int tof2, int tof3, int tof4) {
    // Ne rien faire s'il n'y a aucun client pour ne pas gaspiller de CPU
    if (ws.count() == 0) return; 

    // Création du JSON (Taille augmentée à 512 pour éviter de tronquer les données)
    StaticJsonDocument<512> doc;
    doc["x"] = x;
    doc["y"] = y;
    doc["eL"] = enc_l;
    doc["eR"] = enc_r;
    doc["aL"] = angle_l;
    doc["aR"] = angle_r;
    doc["b"] = battery;
    doc["gz"] = gyro_z;
    doc["c1"] = cpu_load;
    doc["tof1"] = tof1;
    doc["tof2"] = tof2;
    doc["tof3"] = tof3;
    doc["tof4"] = tof4;
    
    String res;
    serializeJson(doc, res);
    
    // Envoi à la page HTML
    ws.textAll(res); 
}

void web_cleanup() {
    ws.cleanupClients();
}