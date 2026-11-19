#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>

void strategy_init();

bool strategy_save(const String& name, JsonArray steps);
bool strategy_read_raw(const String& name, String& outJson);
bool strategy_rename(const String& oldName, const String& newName);
bool strategy_delete(const String& name);
String strategy_list();

void strategy_start(JsonArray steps);
void strategy_stop();
bool strategy_is_running();
void strategy_update();

// Pour le debug visuel côté web
int strategy_get_state();
int strategy_get_step_index();
int strategy_get_step_count();
float strategy_get_target_heading();