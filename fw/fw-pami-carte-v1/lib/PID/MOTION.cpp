#include "motion.h"
#include <math.h>

volatile float robot_x = 0.0f;
volatile float robot_y = 0.0f;
volatile float robot_theta_rad = 0.0f;

static float _wheel_track_mm;
static float _mm_per_step;
static float _steps_per_mm;

static long _last_step_L = 0;
static long _last_step_R = 0;
static bool _first_update = true;

void motion_init(float wheel_diameter_mm, float wheel_track_mm, int steps_per_rev) {
    _wheel_track_mm = wheel_track_mm;
    _mm_per_step = (PI * wheel_diameter_mm) / steps_per_rev;
    _steps_per_mm = 1.0f / _mm_per_step;
    _first_update = true;
}

void motion_set_position(float x, float y, float theta_deg) {
    robot_x = x;
    robot_y = y;
    robot_theta_rad = theta_deg * (PI / 180.0f);
    _first_update = true;
}

void motion_update_odometry(long step_L, long step_R, float absolute_angle_deg) {
    if (_first_update) {
        _last_step_L = step_L;
        _last_step_R = step_R;
        _first_update = false;
        return;
    }

    long delta_steps_L = step_L - _last_step_L;
    long delta_steps_R = step_R - _last_step_R;

    _last_step_L = step_L;
    _last_step_R = step_R;

    float dist_L = delta_steps_L * _mm_per_step;
    float dist_R = delta_steps_R * _mm_per_step;
    float dist_center = (dist_L + dist_R) / 2.0f;

    robot_theta_rad = absolute_angle_deg * (PI / 180.0f);

    robot_x += dist_center * cos(robot_theta_rad);
    robot_y += dist_center * sin(robot_theta_rad);
}

float motion_mm_to_steps(float mm) {
    return mm * _steps_per_mm;
}