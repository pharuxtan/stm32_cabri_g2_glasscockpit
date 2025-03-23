#include "app/components.h"

lv_svg_node_t *svg_general_scales[6];

#define SCALE 0.1f
#define SPACE_BETWEEN 50 + 25

#define MIN_ARROW_ANGLE 120.0f
#define MAX_ARROW_ANGLE 360.0f
#define ARROW_ANGLE_RANGE (MAX_ARROW_ANGLE - MIN_ARROW_ANGLE)

#define SVG_CARBURATOR_TEMPERATURE    0
#define SVG_CYLINDER_HEAD_TEMPERATURE 1
#define SVG_ENGINE_OIL_TEMPERATURE    2
#define SVG_ENGINE_OIL_PRESSURE       3
#define SVG_FUEL_PRESSURE             4
#define SVG_HEAT_BAR                  5

#define ARROW 0

#define TEMP_LEVEL_1 0
#define TEMP_LEVEL_2 1
#define TEMP_LEVEL_3 2
#define TEMP_LEVEL_4 3

const float carburator_temperature_scale_values[5][2] = {
  {000.0f, 0.0000f},
  {385.0f, 0.0833f},
  {407.0f, 0.3333f},
  {430.0f, 0.7083f},
  {450.0f, 1.0000f},
};

const float cylinder_head_temperature_scale_values[3][2] = {
  {000.0f, 0.0000f},
  {260.0f, 0.8458f},
  {280.0f, 1.0000f},
};

const float engine_oil_temperature_scale_values[4][2] = {
  {000.0f, 0.0000f},
  {060.0f, 0.2708f},
  {118.0f, 0.8833f},
  {130.0f, 1.0000f},
};

const float engine_oil_pressure_scale_values[4][2] = {
  {000.0f, 0.0000f},
  {025.0f, 0.1875f},
  {052.0f, 0.4500f},
  {115.0f, 0.9375f},
};

const float fuel_pressure_scale_values[3][2] = {
  {0.0f, 0.0000f},
  {0.3f, 0.0625f},
  {8.0f, 0.9292f},
};

void init_general_scales()
{
  svg_general_scales[SVG_CARBURATOR_TEMPERATURE] 	= load_svg(svg_carburator_temperature_data);
  svg_general_scales[SVG_CYLINDER_HEAD_TEMPERATURE] 	= load_svg(svg_cylinder_head_temperature_data);
  svg_general_scales[SVG_ENGINE_OIL_TEMPERATURE] 	= load_svg(svg_engine_oil_temperature_data);
  svg_general_scales[SVG_ENGINE_OIL_PRESSURE] 		= load_svg(svg_engine_oil_pressure_data);
  svg_general_scales[SVG_FUEL_PRESSURE] 		= load_svg(svg_fuel_pressure_data);
  svg_general_scales[SVG_HEAT_BAR] 			= load_svg(svg_heat_bar_data);

  int offset_x = 37;
  for(int i = 0; i < 5; i++){
    nema_matrix3x3_t *matrix = get_svg_root_matrix(svg_general_scales[i]);
    transform_matrix(matrix, translate, offset_x, 205);
    transform_matrix(matrix, scale, SCALE, SCALE);
    offset_x += SPACE_BETWEEN;
  }

  nema_matrix3x3_t *matrix = get_svg_root_matrix(svg_general_scales[SVG_HEAT_BAR]);
  transform_matrix(matrix, translate, 13, 206);
  transform_matrix(matrix, scale, SCALE, SCALE);
}

void render_general_scales(lv_layer_t *layer)
{
  float carburator_temperature_angle = get_angle_ratio_from_value(app.data.carburator_temp + 400.0f, carburator_temperature_scale_values, 5) * ARROW_ANGLE_RANGE + MIN_ARROW_ANGLE;
  float cylinder_head_temperature_angle = get_angle_ratio_from_value(app.data.cylinder_head_temp, cylinder_head_temperature_scale_values, 3) * ARROW_ANGLE_RANGE + MIN_ARROW_ANGLE;
  float engine_oil_temperature_angle = get_angle_ratio_from_value(app.data.oil_temp, engine_oil_temperature_scale_values, 4) * ARROW_ANGLE_RANGE + MIN_ARROW_ANGLE;
  float engine_oil_pressure_angle = get_angle_ratio_from_value(app.data.oil_pressure, engine_oil_pressure_scale_values, 4) * ARROW_ANGLE_RANGE + MIN_ARROW_ANGLE;
  float fuel_pressure_angle = get_angle_ratio_from_value(app.data.fuel_pressure, fuel_pressure_scale_values, 3) * ARROW_ANGLE_RANGE + MIN_ARROW_ANGLE;

  static char carburator_temperature[8],
	      cylinder_head_temperature[8],
	      engine_oil_temperature[8],
	      engine_oil_pressure[8],
	      fuel_pressure[8];
  sprintf(carburator_temperature, "%.0f", app.data.carburator_temp);
  sprintf(cylinder_head_temperature, "%.0f", app.data.cylinder_head_temp);
  sprintf(engine_oil_temperature, "%.0f", app.data.oil_temp);
  sprintf(engine_oil_pressure, "%.1f", app.data.oil_pressure);
  sprintf(fuel_pressure, "%.1f", app.data.fuel_pressure);

  draw_svg_transform(svg_general_scales[SVG_HEAT_BAR], 4, {
      transform_child(TEMP_LEVEL_1, scale, 1, app.data.carburator_temp_level >= 1);
      transform_child(TEMP_LEVEL_2, scale, 1, app.data.carburator_temp_level >= 2);
      transform_child(TEMP_LEVEL_3, scale, 1, app.data.carburator_temp_level >= 3);
      transform_child(TEMP_LEVEL_4, scale, 1, app.data.carburator_temp_level >= 4);
  });

  draw_svg_transform(svg_general_scales[SVG_CARBURATOR_TEMPERATURE], 1, {
    transform_child(ARROW, rotate, carburator_temperature_angle);
  });

  draw_text_left("HEAT CARB. °C", 12, 4, 257, CANVAS_BG_COLOR, lv_color_hex3(0xfff));
  draw_text_right(carburator_temperature, 18, 95, 237, CANVAS_BG_COLOR, lv_color_hex3(0xfff));

  draw_svg_transform(svg_general_scales[SVG_CYLINDER_HEAD_TEMPERATURE], 1, {
    transform_child(ARROW, rotate, cylinder_head_temperature_angle);
  });

  lv_color_t cylinder_head_temperature_color = app.data.cylinder_head_temp >= 260 ? YELLOW_BG_COLOR : CANVAS_BG_COLOR;
  draw_text_left("CHT °C", 12, 120, 257, CANVAS_BG_COLOR, lv_color_hex3(0xfff));
  draw_text_right(cylinder_head_temperature, 18, 170, 237, cylinder_head_temperature_color, lv_color_hex3(0xfff));

  draw_svg_transform(svg_general_scales[SVG_ENGINE_OIL_TEMPERATURE], 1, {
    transform_child(ARROW, rotate, engine_oil_temperature_angle);
  });

  lv_color_t engine_oil_temperature_color = app.data.engn_oil_temp <= 60 ? YELLOW_BG_COLOR : CANVAS_BG_COLOR;
  draw_text_left("OIL T °C", 12, 193, 257, CANVAS_BG_COLOR, lv_color_hex3(0xfff));
  draw_text_right(engine_oil_temperature, 18, 245, 237, engine_oil_temperature_color, lv_color_hex3(0xfff));

  draw_svg_transform(svg_general_scales[SVG_ENGINE_OIL_PRESSURE], 1, {
    transform_child(ARROW, rotate, engine_oil_pressure_angle);
  });

  lv_color_t engine_oil_pressure_color =  app.data.engn_oil_pressure <= 25 ? RED_BG_COLOR :
					  app.data.engn_oil_pressure <= 50 ? YELLOW_BG_COLOR : CANVAS_BG_COLOR;
  draw_text_left("OIL P bar", 12, 260, 257, CANVAS_BG_COLOR, lv_color_hex3(0xfff));
  draw_text_right(engine_oil_pressure, 18, 320, 237, engine_oil_pressure_color, lv_color_hex3(0xfff));

  draw_svg_transform(svg_general_scales[SVG_FUEL_PRESSURE], 1, {
    transform_child(ARROW, rotate, fuel_pressure_angle);
  });

  lv_color_t fuel_pressure_color = app.data.fuel_pressure <= 0.3 ? YELLOW_BG_COLOR : CANVAS_BG_COLOR;
  draw_text_left("FUEL P bar", 12, 330, 257, CANVAS_BG_COLOR, lv_color_hex3(0xfff));
  draw_text_right(fuel_pressure, 18, 395, 237, fuel_pressure_color, lv_color_hex3(0xfff));
}
