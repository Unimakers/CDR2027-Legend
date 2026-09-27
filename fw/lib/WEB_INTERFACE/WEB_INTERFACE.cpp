#include "WEB_INTERFACE.h"
#include "STRATEGY.h"
#include <WiFi.h>
#include <LittleFS.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

static WebCommandHandler global_command_handler = nullptr;

void web_set_command_handler(WebCommandHandler handler) {
    global_command_handler = handler;
}

void onWsMessage(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    if (type == WS_EVT_DATA) {
        AwsFrameInfo *info = (AwsFrameInfo*)arg;
        if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
            data[len] = 0;

            size_t capacity = min((size_t)(len * 2 + 512), (size_t)16384);
            DynamicJsonDocument doc(capacity);
            if (deserializeJson(doc, (char*)data)) {
                Serial.println("[WS] Erreur de parsing JSON");
                return;
            }

            String cmd = doc["cmd"] | "";

            if (cmd == "strategy") {
                String action = doc["action"] | "";

                if (action == "save") {
                    bool ok = strategy_save(doc["name"] | "strategie", doc["steps"].as<JsonArray>());
                    client->text(String("{\"type\":\"strategy_ack\",\"ok\":") + (ok?"true":"false") + "}");
                }
                else if (action == "load") {
                    String name = doc["name"] | "";
                    String raw;
                    if (strategy_read_raw(name, raw)) {
                        client->text("{\"type\":\"strategy_data\",\"name\":\"" + name + "\",\"steps\":" + raw + "}");
                    }
                }
                else if (action == "rename") {
                    strategy_rename(doc["name"] | "", doc["newName"] | "");
                    client->text("{\"type\":\"strategy_list\",\"list\":\"" + strategy_list() + "\"}");
                }
                else if (action == "delete") {
                    strategy_delete(doc["name"] | "");
                    client->text("{\"type\":\"strategy_list\",\"list\":\"" + strategy_list() + "\"}");
                }
                else if (action == "list") {
                    client->text("{\"type\":\"strategy_list\",\"list\":\"" + strategy_list() + "\"}");
                }
                else if (action == "play") {
                    strategy_start(doc["steps"].as<JsonArray>());
                }
                else if (action == "stop") {
                    strategy_stop();
                }
                return;
            }

            if (global_command_handler != nullptr) {
                String id_str = doc["id"] | "";
                int id = doc["id"] | 0;
                String val = doc["val"] | "";
                global_command_handler(cmd, id, val);
            }
        }
    }
}

bool web_init(const char* ap_ssid, const char* ap_password) {
    WiFi.softAP(ap_ssid, ap_password);
    Serial.print("Point d'acces WiFi lance. IP : ");
    Serial.println(WiFi.softAPIP());

    if (!LittleFS.begin(true)) {
        Serial.println("Erreur Montage LittleFS");
        return false;
    }
    Serial.println("LittleFS OK.");

    ws.onEvent(onWsMessage);
    server.addHandler(&ws);
    server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
    server.begin();
    return true;
}

void web_send_telemetry(float x, float y, long enc_l, long enc_r, float angle_l, float angle_r, float battery, float gyro_z, float gyro_angle, int cpu_load, int tof1, int tof2, int tof3, int tof4) {
    if (ws.count() == 0) return;

    StaticJsonDocument<512> doc;
    doc["x"] = x;
    doc["y"] = y;
    doc["eL"] = enc_l;
    doc["eR"] = enc_r;
    doc["aL"] = angle_l;
    doc["aR"] = angle_r;
    doc["b"] = battery;
    doc["gz"] = gyro_z;
    doc["ga"] = gyro_angle;
    doc["c1"] = cpu_load;
    doc["tof1"] = tof1;
    doc["tof2"] = tof2;
    doc["tof3"] = tof3;
    doc["tof4"] = tof4;

    String res;
    serializeJson(doc, res);
    ws.textAll(res);
}

void web_send_strategy_debug(int state, int stepIdx, int stepCount, float targetHeading, float currentHeading) {
    if (ws.count() == 0) return;
    StaticJsonDocument<256> doc;
    doc["type"] = "strat_debug";
    doc["state"] = state;
    doc["stepIdx"] = stepIdx;
    doc["stepCount"] = stepCount;
    doc["targetHeading"] = targetHeading;
    doc["currentHeading"] = currentHeading;
    doc["error"] = currentHeading - targetHeading;
    String res;
    serializeJson(doc, res);
    ws.textAll(res);
}

void web_cleanup() {
    ws.cleanupClients();
}