#include "app/components.h"

void init_misc()
{

}

void render_misc(lv_layer_t *layer)
{
  if(app.data.engine_fire || app.data.crashed)
  {
    lv_draw_rect_dsc_t fire_rect;
    lv_draw_rect_dsc_init(&fire_rect);
    fire_rect.bg_color = lv_color_hex3(0xf00);
    lv_area_t fire_area = {80, 146, 156, 180};
    lv_draw_rect(layer, &fire_rect, &fire_area);

    draw_text_center("FIRE", 18, 118, 155, lv_color_hex3(0xf00), lv_color_hex3(0x000));
  }
}
