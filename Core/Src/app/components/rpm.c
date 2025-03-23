#include "app/components.h"

lv_svg_node_t *svg_rpm[2];

#define SCALE 0.2f

#define MIN_ARROW_ANGLE 158.0f
#define MAX_ARROW_ANGLE 468.0f
#define ARROW_ANGLE_RANGE (MAX_ARROW_ANGLE - MIN_ARROW_ANGLE)

#define SVG_RPM_SCALE_BAR 0
#define SVG_RPM_ARROW     1

#define BLUE_ARROW  0
#define WHITE_ARROW 1

static const float rotor_rpm_scale_values[13][2] = {
  { 000.0f, 0.0000f },
  { 100.0f, 0.0441f },
  { 150.0f, 0.0698f },
  { 200.0f, 0.0990f },
  { 250.0f, 0.1326f },
  { 300.0f, 0.1724f },
  { 350.0f, 0.2195f },
  { 400.0f, 0.2780f },
  { 450.0f, 0.3578f },
  { 500.0f, 0.4960f },
  { 550.0f, 0.7833f },
  { 600.0f, 0.9251f },
  { 650.0f, 1.0000f },
};

static const float engine_rpm_scale_values[13][2] = {
  { 0000.0f, 0.0000f },
  { 0500.0f, 0.0442f },
  { 0750.0f, 0.0701f },
  { 1000.0f, 0.0994f },
  { 1250.0f, 0.1331f },
  { 1500.0f, 0.1730f },
  { 1750.0f, 0.2203f },
  { 2000.0f, 0.2783f },
  { 2250.0f, 0.3600f },
  { 2500.0f, 0.4927f },
  { 2750.0f, 0.7760f },
  { 3000.0f, 0.9243f },
  { 3500.0f, 1.0000f },
};

void init_rpm()
{
  svg_rpm[SVG_RPM_SCALE_BAR] 	= load_svg(svg_rpm_scale_bar_data);
  svg_rpm[SVG_RPM_ARROW] 	= load_svg(svg_rpm_arrow_data);

  for(int i = 0; i < 2; i++){
    nema_matrix3x3_t *matrix = get_svg_root_matrix(svg_rpm[i]);
    transform_matrix(matrix, translate, 155, 20);
    transform_matrix(matrix, scale, SCALE, SCALE);
  }
}

void render_rpm(lv_layer_t *layer)
{
  float rotor_rpm_angle = get_angle_ratio_from_value(app.data.rotor_rpm, rotor_rpm_scale_values, 13) * ARROW_ANGLE_RANGE + MIN_ARROW_ANGLE;
  float engine_rpm_angle = get_angle_ratio_from_value(app.data.engine_rpm, engine_rpm_scale_values, 13) * ARROW_ANGLE_RANGE + MIN_ARROW_ANGLE;

  static char rotor_rpm[8];
  sprintf(rotor_rpm, "%.0f", app.data.rotor_rpm);

  lv_draw_svg(layer, svg_rpm[SVG_RPM_SCALE_BAR]);

  draw_text_left("2000", 12, 194, 48, CANVAS_BG_COLOR, lv_color_hex(0xa096ff));
  draw_text_left("2500", 12, 246, 55, CANVAS_BG_COLOR, lv_color_hex(0xa096ff));
  draw_text_left("2750", 12, 253, 123, CANVAS_BG_COLOR, lv_color_hex(0xa096ff));
  draw_text_left("3000", 12, 221, 146, CANVAS_BG_COLOR, lv_color_hex(0xa096ff));

  draw_svg_transform(svg_rpm[SVG_RPM_ARROW], 2, {
    transform_child(WHITE_ARROW, rotate, rotor_rpm_angle);
    transform_child(BLUE_ARROW, rotate, engine_rpm_angle);
  });

  draw_text_left("100", 12, 132, 107, CANVAS_BG_COLOR, lv_color_hex3(0xfff));
  draw_text_left("200", 12, 132, 83, CANVAS_BG_COLOR, lv_color_hex3(0xfff));
  draw_text_left("300", 12, 146, 53, CANVAS_BG_COLOR, lv_color_hex3(0xfff));
  draw_text_left("400", 12, 182, 20, CANVAS_BG_COLOR, lv_color_hex3(0xfff));
  draw_text_left("450", 12, 218, 7, CANVAS_BG_COLOR, lv_color_hex3(0xfff));
  draw_text_left("500", 12, 280, 27, CANVAS_BG_COLOR, lv_color_hex3(0xfff));
  draw_text_left("550", 12, 290, 152, CANVAS_BG_COLOR, lv_color_hex3(0xfff));
  draw_text_left("600", 12, 227, 179, CANVAS_BG_COLOR, lv_color_hex3(0xfff));
  draw_text_left("650", 12, 194, 172, CANVAS_BG_COLOR, lv_color_hex3(0xfff));
  draw_text_center("RPM", 12, 176, 158, CANVAS_BG_COLOR, lv_color_hex3(0xfff));

  draw_text_center(rotor_rpm, 18, 176, 135, CANVAS_BG_COLOR, lv_color_hex3(0xfff));
}
