#include "TIMER.h"

PrecisionTimer::PrecisionTimer(float periodMs, float smoothingFactor) 
    : _startTimeUs(0), _stopTimeUs(0), _isMeasuring(false),
      _periodUs(periodMs * 1000.0f), _smoothingFactor(smoothingFactor), _smoothedCpuLoad(0.0f),
      _timerHandle(nullptr), _userCallback(nullptr), _isTimeoutActive(false) {}

PrecisionTimer::~PrecisionTimer() {
    stopTimeout();
}

// Chronomètre & Charge CPU 

void PrecisionTimer::start() {
    _startTimeUs = esp_timer_get_time();
    _stopTimeUs = _startTimeUs;
    _isMeasuring = true;
}

float PrecisionTimer::stop() {
    if (_isMeasuring) {
        _stopTimeUs = esp_timer_get_time();
        _isMeasuring = false;
    }
    return (float)(_stopTimeUs - _startTimeUs) / 1000000.0f;
}

float PrecisionTimer::stopCpuLoad() {
    stop();
    
    int64_t workTimeUs = _stopTimeUs - _startTimeUs;
    float instantLoad = ((float)workTimeUs * 100.0f) / _periodUs;

    // Saturation à 100 % si dépassement de la période
    if (instantLoad > 100.0f) {
        instantLoad = 100.0f;
    }

    // Lissage exponentiel
    _smoothedCpuLoad += _smoothingFactor * (instantLoad - _smoothedCpuLoad);
    
    return _smoothedCpuLoad;
}

float PrecisionTimer::getCpuLoad() const {
    return _smoothedCpuLoad;
}

void PrecisionTimer::setPeriodMs(float periodMs) {
    _periodUs = periodMs * 1000.0f;
}

float PrecisionTimer::getElapsedSeconds() const {
    int64_t now = _isMeasuring ? esp_timer_get_time() : _stopTimeUs;
    return (float)(now - _startTimeUs) / 1000000.0f;
}

uint64_t PrecisionTimer::getElapsedMicros() const {
    int64_t now = _isMeasuring ? esp_timer_get_time() : _stopTimeUs;
    return (uint64_t)(now - _startTimeUs);
}

// Minuteur Haute Priorité

void IRAM_ATTR PrecisionTimer::espTimerTrampoline(void* arg) {
    PrecisionTimer* self = static_cast<PrecisionTimer*>(arg);
    self->_isTimeoutActive = false;
    if (self->_userCallback) {
        self->_userCallback();
    }
}

bool PrecisionTimer::startTimeout(float seconds, TimerCallback cb) {
    stopTimeout();

    _userCallback = cb;

    const esp_timer_create_args_t timerArgs = {
        .callback = &PrecisionTimer::espTimerTrampoline,
        .arg = this,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "match_timeout",
        .skip_unhandled_events = false
    };

    if (esp_timer_create(&timerArgs, &_timerHandle) != ESP_OK) {
        return false;
    }

    uint64_t timeoutUs = (uint64_t)(seconds * 1000000.0f);
    if (esp_timer_start_once(_timerHandle, timeoutUs) != ESP_OK) {
        esp_timer_delete(_timerHandle);
        _timerHandle = nullptr;
        return false;
    }

    _isTimeoutActive = true;
    return true;
}

void PrecisionTimer::stopTimeout() {
    if (_timerHandle != nullptr) {
        esp_timer_stop(_timerHandle);
        esp_timer_delete(_timerHandle);
        _timerHandle = nullptr;
    }
    _isTimeoutActive = false;
}

bool PrecisionTimer::isTimeoutRunning() const {
    return _isTimeoutActive;
}