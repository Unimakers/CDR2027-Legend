#pragma once
#include <Arduino.h>

extern volatile float robot_x;
extern volatile float robot_y;
extern volatile float robot_theta_rad;

void motion_init(float wheel_diameter_mm, float wheel_track_mm, int steps_per_rev);
void motion_set_position(float x, float y, float theta_deg);
void motion_update_odometry(long step_L, long step_R, float absolute_angle_deg);
float motion_mm_to_steps(float mm);