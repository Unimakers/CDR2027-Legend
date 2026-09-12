#include "AS5600.h"

constexpr uint8_t AS5600_ADDR = 0x36;
constexpr uint8_t AS5600_RAW_ANGLE_REG = 0x0E;

AS5600Encoder::AS5600Encoder() {
    _bus = nullptr;
    _last_raw = -1;
    _total_ticks = 0;
}

bool AS5600Encoder::init(TwoWire* i2c_bus) {
    _bus = i2c_bus;
    
    // Test de connexion rapide
    _bus->beginTransmission(AS5600_ADDR);
    return (_bus->endTransmission() == 0);
}

void AS5600Encoder::update() {
    if (_bus == nullptr) return; // Sécurité

    _bus->beginTransmission(AS5600_ADDR);
    _bus->write(AS5600_RAW_ANGLE_REG); 
    
    if (_bus->endTransmission() == 0) {
        _bus->requestFrom((uint8_t)AS5600_ADDR, (uint8_t)2);
        
        if (_bus->available() >= 2) {
            int high = _bus->read();
            int low = _bus->read();
            int raw = ((high << 8) | low) & 0x0FFF; 
            
            // LOGIQUE MULTI-TOURS
            if (_last_raw == -1) {
                _last_raw = raw; // Initialisation au premier tour
            }
            
            int delta = raw - _last_raw;
            
            // Gestion du franchissement du zéro (0 <-> 4095)
            if (delta > 2048) {
                delta -= 4096;
            } else if (delta < -2048) {
                delta += 4096;
            }
            
            _total_ticks += delta;
            _last_raw = raw;
        }
    }
}

float AS5600Encoder::get_angle_degrees() {
    // 4096 ticks = 360 degrés
    return (float)_total_ticks * 360.0f / 4096.0f;
}

long AS5600Encoder::get_total_ticks() {
    return _total_ticks;
}