#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "app/app.h"
#include "stdio.h"

void init_mli();
void init_rpm();
void init_general_scales();
void init_fuel_bar();
void init_bottom_bar();
void init_misc();

void render_mli(lv_layer_t *layer);
void render_rpm(lv_layer_t *layer);
void render_general_scales(lv_layer_t *layer);
void render_fuel_bar(lv_layer_t *layer);
void render_bottom_bar(lv_layer_t *layer);
void render_misc(lv_layer_t *layer);

#endif
