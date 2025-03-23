#include "app/app.h"
#include "app/components.h"
#include "app/usb.h"

app_context_t app;

void init_app()
{
  // Start LCD brightness PWM
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);

  // Init LVGL
  lv_init();
  lv_tick_set_cb(HAL_GetTick);

  // Init Display with LVGL
  init_lvgl_display(0);

  // Init fonts
  app.font12 = lv_tiny_ttf_create_data_ex(font_ttf_data, (size_t)&font_ttf_data_size, 12, LV_FONT_KERNING_NONE, 256);
  app.font14 = lv_tiny_ttf_create_data_ex(font_ttf_data, (size_t)&font_ttf_data_size, 14, LV_FONT_KERNING_NONE, 256);
  app.font16 = lv_tiny_ttf_create_data_ex(font_ttf_data, (size_t)&font_ttf_data_size, 16, LV_FONT_KERNING_NONE, 256);
  app.font18 = lv_tiny_ttf_create_data_ex(font_ttf_data, (size_t)&font_ttf_data_size, 18, LV_FONT_KERNING_NONE, 256);

  // Init canvas
  lv_obj_t *canvas = app.canvas = lv_canvas_create(lv_screen_active());

  static __attribute__((aligned(32))) uint8_t draw_buf[LVGL_CANVAS_WIDTH * LVGL_CANVAS_HEIGHT * 4];
  lv_canvas_set_buffer(canvas, draw_buf, LVGL_CANVAS_WIDTH, LVGL_CANVAS_HEIGHT, LV_COLOR_FORMAT_ARGB8888);

  lv_image_set_rotation(canvas, 1800);
  lv_image_set_antialias(canvas, true);
  lv_obj_center(canvas);

  // Init components
  init_rpm();
  init_mli();
  init_general_scales();
  init_fuel_bar();
  init_bottom_bar();
  init_misc();

  // Init usb communication
  usb_init();
}

static inline void render_canvas()
{
  // Fill the whole canvas with a color
  lv_canvas_fill_bg(app.canvas, CANVAS_BG_COLOR, LV_OPA_COVER);

  // Create a layer
  lv_layer_t layer;
  lv_canvas_init_layer(app.canvas, &layer);

  // Draw components to the layer
  render_rpm(&layer);
  render_mli(&layer);
  render_general_scales(&layer);
  render_fuel_bar(&layer);
  render_bottom_bar(&layer);
  render_misc(&layer);

  // Draw the layer to the canvas
  lv_canvas_finish_layer(app.canvas, &layer);

  // Raster the canvas to the screen
  lv_timer_handler();
}

void app_renderer(void *argument)
{
  while(1)
  {
    if(app.data.power || !app.data.paused)
      render_canvas();

    osDelay(1);
  }
}
