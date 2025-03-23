#include "app/components.h"

lv_svg_node_t *svg_mli[6];

#define SCALE 0.2f

#define MIN_ARROW_ANGLE 127.0f
#define MAX_ARROW_ANGLE 235.0f
#define ARROW_ANGLE_RANGE (MAX_ARROW_ANGLE - MIN_ARROW_ANGLE)

#define SVG_MLI_SCALE_BAR          0
#define SVG_MLI_ARROW              1
#define SVG_MLI_PWR_LIMIT          2
#define SVG_MLI_PWR_WARNING_YELLOW 3
#define SVG_MLI_PWR_WARNING_RED    4
#define SVG_MLI_FLO_LIMIT          5

#define BLUE_ARROW  0
#define WHITE_ARROW 1

static const float mli_scale_values[13][2] = {
  { 000.0f, 0.0000f },
  { 010.0f, 0.0435f },
  { 020.0f, 0.0906f },
  { 030.0f, 0.1415f },
  { 040.0f, 0.2028f },
  { 050.0f, 0.2659f },
  { 060.0f, 0.3399f },
  { 070.0f, 0.4250f },
  { 080.0f, 0.5319f },
  { 090.0f, 0.6601f },
  { 100.0f, 0.8290f },
  { 110.0f, 0.9363f },
  { 120.0f, 1.0000f },
};

void init_mli()
{
  svg_mli[SVG_MLI_SCALE_BAR] 		= load_svg(svg_mli_scale_bar_data);
  svg_mli[SVG_MLI_ARROW] 		= load_svg(svg_mli_arrow_data);
  svg_mli[SVG_MLI_PWR_LIMIT] 		= load_svg(svg_mli_pwr_limit_data);
  svg_mli[SVG_MLI_PWR_WARNING_YELLOW] 	= load_svg(svg_mli_pwr_warning_yellow_data);
  svg_mli[SVG_MLI_PWR_WARNING_RED] 	= load_svg(svg_mli_pwr_warning_red_data);
  svg_mli[SVG_MLI_FLO_LIMIT] 		= load_svg(svg_mli_flo_limit_data);

  for(int i = 0; i < 6; i++){
    nema_matrix3x3_t *matrix = get_svg_root_matrix(svg_mli[i]);
    transform_matrix(matrix, translate, 30, 20);
    transform_matrix(matrix, scale, SCALE, SCALE);
  }
}

void render_mli(lv_layer_t *layer)
{
  float rotor_pwr_angle = get_angle_ratio_from_value(app.data.rotor_pwr * 110.0f, mli_scale_values, 13) * ARROW_ANGLE_RANGE + MIN_ARROW_ANGLE;
  float engine_torque_angle = get_angle_ratio_from_value((app.data.engine_torque + 90.0f) * 100.0f / 581.0f, mli_scale_values, 13) * ARROW_ANGLE_RANGE + MIN_ARROW_ANGLE;

  static char rotor_pwr[8];
  sprintf(rotor_pwr, "%.0f%%", app.data.rotor_pwr * 110.0f);

  lv_draw_svg(layer, svg_mli[SVG_MLI_SCALE_BAR]);
  lv_draw_svg(layer, svg_mli[SVG_MLI_PWR_LIMIT]);

  draw_svg_transform(svg_mli[SVG_MLI_ARROW], 2, {
    transform_child(WHITE_ARROW, rotate, rotor_pwr_angle);
    transform_child(BLUE_ARROW, rotate, engine_torque_angle);
  });

  draw_text_left("120", 12, 45, 10, CANVAS_BG_COLOR, lv_color_hex3(0xfff));
  draw_text_left("100", 12, 23, 32, CANVAS_BG_COLOR, lv_color_hex3(0xfff));
  draw_text_left("80", 12, 11, 84, CANVAS_BG_COLOR, lv_color_hex3(0xfff));
  draw_text_left("60", 12, 14, 120, CANVAS_BG_COLOR, lv_color_hex3(0xfff));
  draw_text_left("40", 12, 24, 145, CANVAS_BG_COLOR, lv_color_hex3(0xfff));
  draw_text_left("20", 12, 38, 164, CANVAS_BG_COLOR, lv_color_hex3(0xfff));
  draw_text_left("0", 12, 58, 175, CANVAS_BG_COLOR, lv_color_hex3(0xfff));
  draw_text_left("MLI", 12, 10, 175, CANVAS_BG_COLOR, lv_color_hex3(0xfff));
  draw_text_left("PWR", 12, 81, 14, CANVAS_BG_COLOR, lv_color_hex3(0xfff));

  draw_text_right(rotor_pwr, 18, 118, 110, CANVAS_BG_COLOR, lv_color_hex3(0xfff));
}
