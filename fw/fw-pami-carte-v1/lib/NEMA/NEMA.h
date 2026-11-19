#pragma once
#include <Arduino.h>

void nema_init();
void nema_set_microstepping(bool ms1_high, bool ms2_high);
void nema_set_profile(uint8_t motor_id, uint32_t speed_hz, uint32_t accel);
void nema_move(uint8_t motor_id, long steps);
void nema_run_forward(uint8_t motor_id);
void nema_run_backward(uint8_t motor_id);
void nema_stop(uint8_t motor_id, bool force_stop = false);
void nema_halt(uint8_t motor_id);
long nema_get_position(uint8_t motor_id);
void nema_move_to(uint8_t motor_id, long absolute_position);
bool nema_is_running(uint8_t motor_id);