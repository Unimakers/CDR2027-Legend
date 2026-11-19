#pragma once
#include <Arduino.h>
#include <Wire.h>

class AS5600Encoder {
private:
    TwoWire* _bus;         // Le bus I2C utilisé (Wire ou Wire1)
    int _last_raw;         // Dernière valeur brute lue
    long _total_ticks;     // Cumul des pas (multi-tours)

public:
    // Constructeur : initialise les variables à zéro
    AS5600Encoder();

    // Attache l'encodeur à son bus I2C
    bool init(TwoWire* i2c_bus);

    // Lit le capteur et met à jour les ticks multi-tours
    void update();

    // Renvoie l'angle total parcouru en degrés
    float get_angle_degrees();
    
    // Renvoie le nombre de ticks bruts (optionnel)
    long get_total_ticks();
};