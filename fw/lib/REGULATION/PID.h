#pragma once

class PIDController {
private:
    float _kp, _ki, _kd;
    float _integral_sum;
    float _previous_error;
    float _output_limit; // Pour ne pas demander 5000 aux moteurs DC (limités à 255)

public:
    // Initialise les gains
    PIDController(float kp, float ki, float kd, float output_limit);
    
    // Met à jour les gains en direct (pratique pour l'interface web !)
    void set_tunings(float kp, float ki, float kd);
    
    // Calcule la nouvelle commande moteur (à appeler à chaque tour de TaskControl)
    float compute(float setpoint, float current_value, float delta_t_sec);
    
    // Vide la mémoire de l'intégrale (indispensable quand le robot s'arrête)
    void reset(); 
};