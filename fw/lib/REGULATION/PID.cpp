#include "pid.h"

PIDController::PIDController(float kp, float ki, float kd, float output_limit) {
    _kp = kp;
    _ki = ki;
    _kd = kd;
    _output_limit = output_limit;
    reset();
}

void PIDController::set_tunings(float kp, float ki, float kd) {
    _kp = kp;
    _ki = ki;
    _kd = kd;
}

float PIDController::compute(float setpoint, float current_value, float delta_t_sec) {
    if (delta_t_sec <= 0.0f) return 0.0f; // Sécurité anti-division par zéro

    float error = setpoint - current_value;

    // Calcul de l'intégrale (avec Anti-Windup pour éviter l'emballement)
    _integral_sum += error * delta_t_sec;
    
    // Limitation de l'intégrale pour ne pas saturer la commande
    float max_integral = _output_limit / (_ki > 0.001f ? _ki : 1.0f);
    if (_integral_sum > max_integral) _integral_sum = max_integral;
    if (_integral_sum < -max_integral) _integral_sum = -max_integral;

    // Calcul de la dérivée
    float derivative = (error - _previous_error) / delta_t_sec;

    // Calcul de la sortie finale
    float output = (_kp * error) + (_ki * _integral_sum) + (_kd * derivative);

    // Sauvegarde pour le prochain tour
    _previous_error = error;

    // Saturation de la sortie (Clamping)
    if (output > _output_limit) return _output_limit;
    if (output < -_output_limit) return -_output_limit;
    
    return output;
}

void PIDController::reset() {
    _integral_sum = 0.0f;
    _previous_error = 0.0f;
}