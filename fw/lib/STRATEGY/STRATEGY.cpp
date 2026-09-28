#include "STRATEGY.h"
#include "MOTION.h"
#include "NEMA.h"
#include "SERVO.h"
#include "MPU9250.h"
#include <LittleFS.h>
#include <vector>
#include <math.h>

enum class StepType : uint8_t { MOVE, ACTION };
enum class RunState : uint8_t { IDLE, ROTATE_TO_TARGET, DRIVING, ROTATE_FINAL, ACTION_WAIT, ACTION_INSTANT, WAIT_STOP };

struct StrategyStep {
    StepType type;
    float x = 0, y = 0, theta = 0;
    String cmd; long param = 0;
};

static std::vector<StrategyStep> _seq;
static size_t _idx = 0;
static bool _running = false;
static RunState _state = RunState::IDLE;
static unsigned long _action_start_ms = 0;
static unsigned long _step_start_ms = 0;
static float _target_heading_deg = 0;

// _seq/_state sont modifies par le websocket (tache AsyncTCP) et lus par TaskControl
static SemaphoreHandle_t _mutex = nullptr;

// Repere de la carte web : X vers la droite (2 m), Y vers le haut (3 m), angles positifs a gauche.
// Convention : 0 deg = face a droite, 90 deg = face en haut, 180 deg = face a gauche.
// Au depart, le robot est pose en (0,0) face au haut de la carte (+Y) => cap 90 deg.
static const float START_HEADING_DEG = 90.0f;

static const float ANGLE_TOLERANCE_DEG = 3.0f;
static const float ROTATE_MAX_SPEED_STEPS = 1500.0f;
static const float ROTATE_MIN_SPEED_STEPS = 400.0f;
static const float ROTATE_SLOWDOWN_ANGLE_DEG = 20.0f;
static const uint32_t ROTATE_ACCEL_STEPS_S2 = 4000;

static const uint32_t DRIVE_SPEED_STEPS_S = 4000;
static const uint32_t DRIVE_ACCEL_STEPS_S2 = 2000;

static const unsigned long ROTATE_TIMEOUT_MS = 3000;
static const unsigned long DRIVE_TIMEOUT_MS = 6000;
static const unsigned long STOP_TIMEOUT_MS = 500;

// En dessous de cette distance, le point est considere comme atteint (cap atan2 instable)
static const float MIN_MOVE_DIST_MM = 15.0f;

void strategy_init() {
    if (!_mutex) _mutex = xSemaphoreCreateMutex();
    if (!LittleFS.exists("/strategies")) LittleFS.mkdir("/strategies");
}

bool strategy_save(const String& name, JsonArray steps) {
    if (name.length() == 0) return false;
    File f = LittleFS.open("/strategies/" + name + ".json", "w");
    if (!f) return false;
    serializeJson(steps, f);
    f.close();
    return true;
}

bool strategy_read_raw(const String& name, String& outJson) {
    File f = LittleFS.open("/strategies/" + name + ".json", "r");
    if (!f) return false;
    outJson = f.readString();
    f.close();
    return true;
}

bool strategy_rename(const String& oldName, const String& newName) {
    if (oldName.length() == 0 || newName.length() == 0) return false;
    return LittleFS.rename("/strategies/" + oldName + ".json", "/strategies/" + newName + ".json");
}

bool strategy_delete(const String& name) {
    return LittleFS.remove("/strategies/" + name + ".json");
}

String strategy_list() {
    String out;
    File root = LittleFS.open("/strategies");
    if (!root || !root.isDirectory()) return out;
    File file = root.openNextFile();
    while (file) {
        String n = String(file.name());
        int slash = n.lastIndexOf('/');
        if (slash != -1) n = n.substring(slash + 1);
        n.replace(".json", "");
        if (out.length() > 0) out += ",";
        out += n;
        file = root.openNextFile();
    }
    return out;
}

static bool step_from_json(JsonObject obj, StrategyStep& out) {
    String type = obj["type"] | "";
    if (type == "MOVE") {
        out.type = StepType::MOVE;
        out.x = obj["x"] | 0.0f; out.y = obj["y"] | 0.0f; out.theta = obj["theta"] | 0.0f;
        return true;
    } else if (type == "ACTION") {
        out.type = StepType::ACTION;
        out.cmd = String((const char*)(obj["cmd"] | ""));
        out.param = obj["param"] | 0;
        return true;
    }
    return false;
}

static float wrap_180(float deg) {
    while (deg > 180.0f) deg -= 360.0f;
    while (deg < -180.0f) deg += 360.0f;
    return deg;
}

// Convention validee par test manuel : err>0 (augmenter angle, tourner a gauche)
// -> moteur gauche(id 2) avance, moteur droit(id 1) recule
static float compute_rotation_speed(float err) {
    float absErr = fabs(err);
    float speed;
    if (absErr > ROTATE_SLOWDOWN_ANGLE_DEG) {
        speed = ROTATE_MAX_SPEED_STEPS;
    } else {
        speed = ROTATE_MIN_SPEED_STEPS +
                (ROTATE_MAX_SPEED_STEPS - ROTATE_MIN_SPEED_STEPS) * (absErr / ROTATE_SLOWDOWN_ANGLE_DEG);
    }
    return (err >= 0) ? speed : -speed;
}

static void set_rotation_speeds(float w) {
    if (w >= 0) { nema_set_profile(2, (uint32_t)w, ROTATE_ACCEL_STEPS_S2); nema_run_backward(2); }
    else        { nema_set_profile(2, (uint32_t)(-w), ROTATE_ACCEL_STEPS_S2); nema_run_forward(2); }
    if (w >= 0) { nema_set_profile(1, (uint32_t)w, ROTATE_ACCEL_STEPS_S2); nema_run_forward(1); }
    else        { nema_set_profile(1, (uint32_t)(-w), ROTATE_ACCEL_STEPS_S2); nema_run_backward(1); }
}

static void execute_action(const StrategyStep& s) {
    if (s.cmd == "ATTENTE") {
        _state = RunState::ACTION_WAIT; _action_start_ms = millis();
    } else if (s.cmd == "SERVO_ANGLE") {
        servos_set_angle(0, (uint8_t)s.param);
        _state = RunState::ACTION_INSTANT;
    } else if (s.cmd == "DEPLOY_BRAS") {
        servos_set_angle(1, 180); // TODO : adapter à ton vrai actionneur
        _state = RunState::ACTION_INSTANT;
    } else if (s.cmd == "RETRACT_BRAS") {
        servos_set_angle(1, 0);
        _state = RunState::ACTION_INSTANT;
    } else {
        _state = RunState::ACTION_INSTANT;
    }
}

void strategy_start(JsonArray steps) {
    xSemaphoreTake(_mutex, portMAX_DELAY);
    _seq.clear();
    for (JsonObject obj : steps) {
        StrategyStep s;
        if (step_from_json(obj, s)) _seq.push_back(s);
    }
    Serial.printf("[STRATEGY] %d etapes chargees, demarrage\n", _seq.size());
    if (_seq.empty()) { _running = false; xSemaphoreGive(_mutex); return; }

    mpu_set_angle(START_HEADING_DEG);
    motion_set_position(0.0f, 0.0f, START_HEADING_DEG);

    _idx = 0; _state = RunState::IDLE; _running = true;
    xSemaphoreGive(_mutex);
}

void strategy_stop() {
    xSemaphoreTake(_mutex, portMAX_DELAY);
    _running = false; _state = RunState::IDLE;
    nema_halt(1); nema_halt(2);
    xSemaphoreGive(_mutex);
}

bool strategy_is_running() { return _running; }

static const char* STATE_NAMES[] = {"IDLE","ROTATE_TO_TARGET","DRIVING","ROTATE_FINAL","ACTION_WAIT","ACTION_INSTANT","WAIT_STOP"};

// Toute transition passe par ici : trace Serial exploitable meme quand l'interface web decroche
static void set_state(RunState next) {
    if (next == _state) return;
    Serial.printf("[STRATEGY] etape %d/%d %s -> %s | cap %.1f cible %.1f | pos %ld/%ld run %d/%d\n",
                  (int)_idx + 1, (int)_seq.size(), STATE_NAMES[(int)_state], STATE_NAMES[(int)next],
                  mpu_get_angle_z(), _target_heading_deg,
                  nema_get_position(1), nema_get_position(2),
                  nema_is_running(1), nema_is_running(2));
    _state = next;
}

static void abort_current_step(const char* reason) {
    Serial.printf("[STRATEGY] Etape %d abandonnee (%s)\n", (int)_idx, reason);
    nema_halt(1); nema_halt(2);
    set_state(RunState::IDLE); _idx++;
}

// Fin d'un MOVE : rotation finale (theta) seulement si le prochain pas n'est pas un MOVE.
// Si le suivant est un MOVE, il orientera lui-meme le robot vers sa cible.
static void finish_move() {
    bool next_is_move = (_idx + 1 < _seq.size()) && _seq[_idx + 1].type == StepType::MOVE;
    if (next_is_move) {
        set_state(RunState::IDLE);
        _idx++;
    } else {
        _step_start_ms = millis();
        set_state(RunState::ROTATE_FINAL);
    }
}

static void strategy_step();

void strategy_update() {
    if (xSemaphoreTake(_mutex, pdMS_TO_TICKS(5)) != pdTRUE) return;
    strategy_step();
    xSemaphoreGive(_mutex);
}

static void strategy_step() {
    if (!_running) return;
    if (_idx >= _seq.size()) {
        Serial.println("[STRATEGY] Sequence terminee");
        _running = false; nema_halt(1); nema_halt(2); return;
    }

    StrategyStep& s = _seq[_idx];

    if (s.type == StepType::MOVE) {
        float dx = (s.x * 1000.0f) - robot_x;
        float dy = (s.y * 1000.0f) - robot_y;

        if (_state == RunState::IDLE) {
            if (sqrt(dx*dx + dy*dy) < MIN_MOVE_DIST_MM) {
                finish_move();      // deja arrive : pas de rotation vers un cap indefini
                return;
            }
            _target_heading_deg = atan2(dy, dx) * 180.0f / PI;
            _step_start_ms = millis();
            set_state(RunState::ROTATE_TO_TARGET);
        }

        if (_state == RunState::ROTATE_TO_TARGET || _state == RunState::ROTATE_FINAL) {
            if (millis() - _step_start_ms > ROTATE_TIMEOUT_MS) { abort_current_step("timeout rotation"); return; }

            float targetAngle = (_state == RunState::ROTATE_TO_TARGET) ? _target_heading_deg : s.theta;
            float err = wrap_180(targetAngle - mpu_get_angle_z());

            if (fabs(err) < ANGLE_TOLERANCE_DEG) {
                // stopMove() (rampe) ne stoppait pas le mode runForward/runBackward :
                // arret immediat, sans risque a la vitesse de fin de rotation (~550 pas/s)
                nema_halt(1); nema_halt(2);
                if (_state == RunState::ROTATE_TO_TARGET) {
                    // On attend l'arret complet des moteurs avant de lancer la ligne droite
                    _step_start_ms = millis();
                    set_state(RunState::WAIT_STOP);
                } else {
                    set_state(RunState::IDLE); _idx++;
                }
            } else {
                float w = compute_rotation_speed(err);
                set_rotation_speeds(w);
                static unsigned long last_log_ms = 0;
                if (millis() - last_log_ms > 200) {
                    last_log_ms = millis();
                    Serial.printf("[STRATEGY]   rot cap %.1f err %.1f w %.0f | pos %ld/%ld\n",
                                  mpu_get_angle_z(), err, w, nema_get_position(1), nema_get_position(2));
                }
            }
        }
        else if (_state == RunState::WAIT_STOP) {
            if (millis() - _step_start_ms > STOP_TIMEOUT_MS) { abort_current_step("moteurs non arretes"); return; }
            if (!nema_is_running(1) && !nema_is_running(2)) {
                float distance_mm = sqrt(dx*dx + dy*dy);
                long deltaSteps = (long)motion_mm_to_steps(distance_mm);

                // Cible absolue depuis la position reelle : apres un runForward/runBackward,
                // un move() relatif peut partir d'une cible obsolete et defaire la rotation
                nema_set_profile(1, DRIVE_SPEED_STEPS_S, DRIVE_ACCEL_STEPS_S2);
                nema_set_profile(2, DRIVE_SPEED_STEPS_S, DRIVE_ACCEL_STEPS_S2);
                nema_move_to(1, nema_get_position(1) + deltaSteps);
                nema_move_to(2, nema_get_position(2) + deltaSteps);

                _step_start_ms = millis();
                set_state(RunState::DRIVING);
            }
        }
        else if (_state == RunState::DRIVING) {
            if (millis() - _step_start_ms > DRIVE_TIMEOUT_MS) {
                abort_current_step("timeout deplacement (blocage moteur ?)");
                return;
            }
            if (!nema_is_running(1) && !nema_is_running(2)) {
                finish_move();      // au lieu de ROTATE_FINAL systematique
            }
        }
    } else {
        if (_state == RunState::IDLE) execute_action(s);
        else if (_state == RunState::ACTION_WAIT) {
            if (millis() - _action_start_ms >= (unsigned long)s.param) { set_state(RunState::IDLE); _idx++; }
        } else if (_state == RunState::ACTION_INSTANT) {
            set_state(RunState::IDLE); _idx++;
        }
    }
}

int strategy_get_state() { return (int)_state; }
int strategy_get_step_index() { return (int)_idx; }
int strategy_get_step_count() { return (int)_seq.size(); }
float strategy_get_target_heading() { return _target_heading_deg; }
