#pragma once

#include <Arduino.h>
#include "esp_timer.h"

typedef void (*TimerCallback)(void);

class PrecisionTimer {
public:
    // periodMs : période nominale de ta boucle (ex: 20.0f pour 50 Hz)
    // smoothingFactor : facteur de lissage entre 0.0 et 1.0 (0.05 par défaut)
    explicit PrecisionTimer(float periodMs = 20.0f, float smoothingFactor = 0.05f);
    ~PrecisionTimer();

    // --- 1. Chronomètre & Calcul CPU ---
    void start();
    float stop();                  // Arrête et renvoie le temps écoulé en secondes
    float stopCpuLoad();           // Arrête, met à jour et renvoie la charge CPU lissée en %
    
    float getCpuLoad() const;      // Renvoie la dernière charge calculée sans arrêter
    float getElapsedSeconds() const;
    uint64_t getElapsedMicros() const;
    void setPeriodMs(float periodMs);

    // --- 2. Minuteur asynchrone prioritaire ---
    bool startTimeout(float seconds, TimerCallback cb);
    void stopTimeout();
    bool isTimeoutRunning() const;

private:
    int64_t _startTimeUs;
    int64_t _stopTimeUs;
    bool _isMeasuring;

    // Gestion de charge CPU
    float _periodUs;
    float _smoothingFactor;
    float _smoothedCpuLoad;

    // ESP-Timer
    esp_timer_handle_t _timerHandle;
    TimerCallback _userCallback;
    bool _isTimeoutActive;

    static void IRAM_ATTR espTimerTrampoline(void* arg);
};