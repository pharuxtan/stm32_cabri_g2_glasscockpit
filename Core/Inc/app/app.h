#ifndef APP_H
#define APP_H

#include "app_freertos.h"
#include "lvgl/lvgl.h"
#include "libs/lvgl_display.h"
#include "lvgl/src/libs/tiny_ttf/lv_tiny_ttf.h"
#include "libs/helper.h"
#include "app/assets.h"
#include "stdlib.h"

#define LVGL_CANVAS_WIDTH  400
#define LVGL_CANVAS_HEIGHT 300

#define CANVAS_BG_COLOR lv_color_hex3(0x000)
#define YELLOW_BG_COLOR lv_color_hex(0xe6cf01)
#define RED_BG_COLOR lv_color_hex(0xd64c1d)

#define LCD_BRIGHTNESS 100

typedef struct {
  uint8_t power;                 // sim/cockpit2/switches/avionics_power_on
  uint8_t carburator_temp_level; // vskylabs/g2/carbheat_level
  uint8_t engine_fire;           // sim/cockpit2/annunciators/engine_fire
  uint8_t crashed;               // sim/flightmodel2/misc/has_crashed
  uint8_t paused;                // sim/time/paused
  float carburator_temp;         // vskylabs/g2/carbair_temp
  float battery_voltage;         // sim/cockpit2/electrical/battery_voltage_indicated_volts[0]
  float outside_air_temp;        // sim/cockpit2/temperature/outside_air_temp_degc
  float exhaust_gas_temp;        // sim/cockpit2/engine/indicators/EGT_deg_C[0]
  float fuel_quantity;           // sim/cockpit2/fuel/fuel_quantity[0]
  float fuel_flow;               // sim/cockpit2/engine/indicators/fuel_flow_kg_sec[0]
  float cylinder_head_temp;      // sim/cockpit2/engine/indicators/CHT_deg_C[0]
  float oil_temp;                // sim/cockpit2/engine/indicators/oil_temperature_deg_C[0]
  float engn_oil_temp;           // sim/flightmodel/engine/ENGN_oil_temp_c[0]
  float oil_pressure;            // sim/cockpit2/engine/indicators/oil_pressure_psi[0]
  float engn_oil_pressure;       // sim/flightmodel/engine/ENGN_oil_press_psi[0]
  float fuel_pressure;           // sim/cockpit2/engine/indicators/fuel_pressure_psi[0]
  float rotor_pwr;               // sim/cockpit2/engine/actuators/prop_ratio[0]
  float rotor_rpm;               // sim/cockpit2/engine/indicators/prop_speed_rpm[0]
  float engine_torque;           // sim/cockpit2/engine/indicators/torque_n_mtr[0]
  float engine_rpm;              // sim/cockpit2/engine/indicators/engine_speed_rpm[0]
  float local_time_hours;        // sim/cockpit2/clock_timer/local_time_hours
  float local_time_minutes;      // sim/cockpit2/clock_timer/local_time_minutes
  uint64_t flight_time;          // sim/time/total_flight_time_sec
} app_data_t;

typedef struct {
  lv_obj_t *canvas;
  lv_font_t *font12;
  lv_font_t *font14;
  lv_font_t *font16;
  lv_font_t *font18;

  app_data_t data;
} app_context_t;

extern app_context_t app;

void init_app();

#endif
