#include "OTA.h"
#include <ArduinoOTA.h>
#include <LittleFS.h>

static OtaStartHandler start_handler = nullptr;
static volatile bool ota_active = false;
static bool fs_unmounted = false;
static int last_percent = -1;

void ota_set_start_handler(OtaStartHandler handler) {
    start_handler = handler;
}

void ota_init(const char* hostname) {
    ArduinoOTA.setHostname(hostname);

    ArduinoOTA.onStart([]() {
        ota_active = true;
        last_percent = -1;
        if (start_handler != nullptr) start_handler();

        // Image LittleFS (uploadfs) : la partition doit etre demontee avant d'etre reecrite
        if (ArduinoOTA.getCommand() == U_SPIFFS) {
            LittleFS.end();
            fs_unmounted = true;
            Serial.println("[OTA] Debut mise a jour LittleFS");
        } else {
            Serial.println("[OTA] Debut mise a jour firmware");
        }
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        int percent = (int)(progress * 100ULL / total);
        if (percent / 10 != last_percent / 10) Serial.printf("[OTA] %d%%\n", percent);
        last_percent = percent;
    });

    ArduinoOTA.onEnd([]() {
        Serial.println("[OTA] Termine, redemarrage");
    });

    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("[OTA] Erreur %u\n", error);
        // Pas de redemarrage apres un echec : on remonte le LittleFS pour garder l'interface web
        if (fs_unmounted) {
            LittleFS.begin(true);
            fs_unmounted = false;
        }
        ota_active = false;
    });

    ArduinoOTA.begin();
    Serial.printf("[OTA] Pret : %s.local\n", hostname);
}

void ota_handle() {
    ArduinoOTA.handle();
}

bool ota_is_active() {
    return ota_active;
}
