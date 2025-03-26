#ifndef LVGL_DISPLAY_H
#define LVGL_DISPLAY_H

#include "main.h"
#include "string.h"
#include "helper.h"
#include "lvgl/lvgl.h"
#include "lvgl/src/core/lv_global.h"
#include "lvgl/src/draw/nema_gfx/lv_draw_nema_gfx.h"
#include "nema_cmdlist.h"
#include "nema_graphics.h"
#include "nema_blender.h"

#define FLUSH_DMA2D 0
#define FLUSH_NEMA  1

// -------- CONFIGURATION --------

// To change the LTDC/LVGL width and height, change the constants values on IOC User Constants

// The method used to flush the image from lvgl to the ltdc display
#define FLUSH_METHOD FLUSH_NEMA

// The color format of the DMA2D intermediate buffer
#define INTERMEDIATE_COLOR_FORMAT DMA2D_INPUT_RGB565

// ------ END CONFIGURATION ------

#if LV_USE_NEMA_GFX == 0 && FLUSH_METHOD == FLUSH_NEMA
#error LV_USE_NEMA_GFX must be enabled to use nema flush method
#endif

#if LV_COLOR_DEPTH == 16
#define LVGL_COLOR_BYTE_SIZE 2
#define LVGL_COLOR_ALIGNMENT 16
#elif LV_COLOR_DEPTH == 24
#define LVGL_COLOR_BYTE_SIZE 3
#define LVGL_COLOR_ALIGNMENT 32
#elif LV_COLOR_DEPTH == 32
#define LVGL_COLOR_BYTE_SIZE 4
#define LVGL_COLOR_ALIGNMENT 32
#endif

#if FLUSH_METHOD == FLUSH_DMA2D

#if INTERMEDIATE_COLOR_FORMAT == DMA2D_INPUT_RGB565 \
 || INTERMEDIATE_COLOR_FORMAT == DMA2D_INPUT_ARGB1555 \
 || INTERMEDIATE_COLOR_FORMAT == DMA2D_INPUT_ARGB4444
#define INTERMEDIATE_COLOR_BYTE_SIZE 2
#define INTERMEDIATE_COLOR_ALIGNMENT 16
#elif INTERMEDIATE_COLOR_FORMAT == DMA2D_INPUT_RGB888
#define INTERMEDIATE_COLOR_BYTE_SIZE 3
#define INTERMEDIATE_COLOR_ALIGNMENT 32
#elif INTERMEDIATE_COLOR_FORMAT == DMA2D_INPUT_ARGB8888
#define INTERMEDIATE_COLOR_BYTE_SIZE 4
#define INTERMEDIATE_COLOR_ALIGNMENT 32
#endif

#define BLEND_SHIFT_MAX_VAL MAX(LTDC_DISPLAY_WIDTH, MAX(LTDC_DISPLAY_HEIGHT, MAX(LVGL_DISPLAY_WIDTH, LVGL_DISPLAY_HEIGHT)))
#define BLEND_SHIFT (32 - BITS_TO_REPRESENT(BLEND_SHIFT_MAX_VAL - 1))

#endif

#define RATIO_WIDTH(W) ((uint32_t)(roundf((float)W) * ((float)LTDC_DISPLAY_WIDTH) / ((float)LVGL_DISPLAY_WIDTH)))
#define RATIO_HEIGHT(H) ((uint32_t)(roundf((float)H) * ((float)LTDC_DISPLAY_HEIGHT) / ((float)LVGL_DISPLAY_HEIGHT)))

typedef struct {
  uint16_t area_x; // Area horizontal position
  uint16_t area_y; // Area vertical position
  uint16_t area_width; // Area width
  uint16_t area_height; // Area height

  uint16_t buf_width; // Buffer width
  uint16_t buf_height; // Buffer height
  uint8_t buf_color_format; // Buffer color format
  void *buf_address; // Buffer address
} display_rectangle_t;

#if FLUSH_METHOD == FLUSH_DMA2D
typedef enum {
  DISPLAY_STEP_IDLE,
  DISPLAY_STEP_FIRST_PASS,
  DISPLAY_STEP_SECOND_PASS
} display_step_t;

typedef struct {
  uint8_t buf_color_format;
  void *buf_address;
} display_intermediate_t;

typedef struct {
  uint16_t counter;
  uint16_t source_stride;
  uint16_t destination_stride;
  uint32_t blend_coeff;
  uint64_t blend_index;
  void *base_address;
} display_loop_t;

typedef struct {
  display_step_t step;
  display_loop_t loop;
  display_intermediate_t intermediate;
} display_dma2d_t;
#elif FLUSH_METHOD == FLUSH_NEMA
typedef struct {
  nema_cmdlist_t cl; // NemaGFX command list
  nema_cmdlist_t *p_cl; // Pointer to the NemaGFX command list
  int32_t submission_id; // Backup ID to wait the flush
} display_nema_t;
#endif

typedef struct {
  lv_display_t *lv_disp; // The lvgl display
  uint8_t layer_index; // The index of the LTDC layer
  display_rectangle_t source; // The source buffer
  display_rectangle_t destination; // The destination buffer
#if FLUSH_METHOD == FLUSH_DMA2D
  display_dma2d_t dma2d;
#elif FLUSH_METHOD == FLUSH_NEMA
  display_nema_t nema;
#endif
} display_context_t;

extern display_context_t display;

lv_display_t *init_lvgl_display();

#endif // LVGL_DISPLAY_H
