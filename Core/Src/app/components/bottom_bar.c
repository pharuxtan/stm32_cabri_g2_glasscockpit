#include "app/components.h"

lv_svg_node_t *svg_bottom_bar[1];

#define SCALE 1.0f

#define SVG_BOTTOM_BAR 0

#define BATTERY 0

void init_bottom_bar()
{
  svg_bottom_bar[SVG_BOTTOM_BAR] = load_svg(svg_bottom_bar_data);

  for(int i = 0; i < 1; i++){
    nema_matrix3x3_t *matrix = get_svg_root_matrix(svg_bottom_bar[i]);
    transform_matrix(matrix, translate, 0, 270);
    transform_matrix(matrix, scale, SCALE, SCALE);
  }
}

void render_bottom_bar(lv_layer_t *layer)
{
  static char oat_c[8], egt_c[8];
  sprintf(oat_c, "%.0f°C", app.data.outside_air_temp);
  sprintf(egt_c, "%.0f°C", app.data.exhaust_gas_temp);

  uint8_t flight_time_seconds = app.data.flight_time % 60;
  uint8_t flight_time_minutes = (app.data.flight_time / 60) % 60;
  uint8_t flight_time_hours = (app.data.flight_time / 3600) % 60;
  static char flight_time[16];
  sprintf(flight_time, "%01u:%02u:%02u", flight_time_hours, flight_time_minutes, flight_time_seconds);

  static char local_time[16];
  sprintf(local_time, "%02.0f:%02.0f", app.data.local_time_hours, app.data.local_time_minutes);

  draw_text_center("BATT.", 12, 42, 273, CANVAS_BG_COLOR, lv_color_hex3(0xfff));
  draw_svg_transform(svg_bottom_bar[SVG_BOTTOM_BAR], 1, {
    transform_child(BATTERY, scale, 1, app.data.battery_voltage >= 0);
    transform_child(BATTERY, translate, 26, 0);
  });

  draw_text_center("OAT", 12, 111, 273, CANVAS_BG_COLOR, lv_color_hex3(0xfff));
  draw_text_center(oat_c, 12, 111, 286, CANVAS_BG_COLOR, lv_color_hex3(0xfff));

  draw_text_center("EGT", 12, 233, 273, CANVAS_BG_COLOR, lv_color_hex3(0xfff));
  draw_text_center(egt_c, 12, 233, 286, CANVAS_BG_COLOR, lv_color_hex3(0xfff));

  draw_text_center(local_time, 16, 293, 277, CANVAS_BG_COLOR, lv_color_hex3(0xfff));
  draw_text_center(flight_time, 16, 362, 277, CANVAS_BG_COLOR, lv_color_hex3(0xfff));
}
