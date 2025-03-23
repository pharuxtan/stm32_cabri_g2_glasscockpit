#include "app/components.h"

lv_svg_node_t *svg_fuel_bar[3];

#define SCALE 0.25f
#define SCALE_SUB 0.4f

#define SVG_FUEL_BAR_STROKE 0
#define SVG_FUEL_BAR_MASK 1
#define SVG_FUEL_SUB_TEXT 2

#define ARROW 0
#define MASK  0

void init_fuel_bar()
{
  svg_fuel_bar[SVG_FUEL_BAR_STROKE] = load_svg(svg_fuel_bar_stroke_data);
  svg_fuel_bar[SVG_FUEL_BAR_MASK] = load_svg(svg_fuel_bar_mask_data);
  svg_fuel_bar[SVG_FUEL_SUB_TEXT] = load_svg(svg_fuel_sub_text_data);

  for(int i = 0; i < 2; i++){
    nema_matrix3x3_t *matrix = get_svg_root_matrix(svg_fuel_bar[i]);
    transform_matrix(matrix, translate, 335, 12);
    transform_matrix(matrix, scale, SCALE, SCALE);
  }

  nema_matrix3x3_t *matrix = get_svg_root_matrix(svg_fuel_bar[SVG_FUEL_SUB_TEXT]);
  transform_matrix(matrix, translate, 310, 174);
  transform_matrix(matrix, scale, SCALE_SUB, SCALE_SUB);
}

void render_fuel_bar(lv_layer_t *layer)
{
  float fuel_quantity = app.data.fuel_quantity * 170.0f / 120.0f;

  float height = (1.0f - (fuel_quantity / 180.0f)) * 555.0f;
  if(height > 555) height = 555.0f;

  static char fuel_qty[8];
  sprintf(fuel_qty, "%.0f", fuel_quantity);

  static char fuel_flow[8];
  sprintf(fuel_flow, "%.0f", app.data.fuel_flow);

  draw_svg_transform(svg_fuel_bar[SVG_FUEL_BAR_STROKE], 1, {
    transform_child(ARROW, translate, 0, height);
  });

  draw_svg_transform(svg_fuel_bar[SVG_FUEL_BAR_MASK], 1, {
    transform_child(MASK, scale, 1, height);
  });

  draw_text_left("180", 12, 363, 6, CANVAS_BG_COLOR, lv_color_hex3(0xfff));
  draw_text_left("0", 12, 363, 146, CANVAS_BG_COLOR, lv_color_hex3(0xfff));

  draw_text_left(fuel_qty, 16, 363, 4 +  height * SCALE, CANVAS_BG_COLOR, lv_color_hex3(0xfff));

  draw_text_right("FUEL liters", 12, 398, 160, CANVAS_BG_COLOR, lv_color_hex3(0xfff));

  // right sim/cockpit2/fuel/quantity
  // left sim/cockpit2/engine/indicators/fuel_flow_kg_sec
  lv_draw_svg(layer, svg_fuel_bar[SVG_FUEL_SUB_TEXT]);

  draw_text_right(fuel_flow, 14, 343, 182, CANVAS_BG_COLOR, lv_color_hex(0x6e91ff));
  draw_text_left("kg/s", 12, 348, 184, CANVAS_BG_COLOR, lv_color_hex(0x6e91ff));
}
