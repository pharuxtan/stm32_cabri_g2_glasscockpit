#ifndef HELPER_H
#define HELPER_H

#include "math.h"
#include "lvgl/lvgl.h"
#include "nema_matrix3x3.h"

float get_angle_ratio_from_value(float value, const float (*scale_values)[2], size_t len);

#define draw_text_left(str, font_size, _x, _y, bg_clr, txt_clr) ({ \
  /* Get the size of the text and create an area */ \
  lv_point_t size; \
  lv_text_get_size(&size, str, app.font##font_size, 1, 0, LVGL_CANVAS_WIDTH, LV_TEXT_FLAG_NONE); \
  lv_area_t area = {_x, _y, _x + size.x - 1, _y + size.y - 1}; \
  /* Draw a rectangle behind the text */ \
  lv_draw_rect_dsc_t rect_dsc; \
  lv_draw_rect_dsc_init(&rect_dsc); \
  rect_dsc.bg_color = bg_clr; \
  lv_draw_rect(layer, &rect_dsc, &area); \
  /* Draw the text */ \
  lv_draw_label_dsc_t txt_dsc; \
  lv_draw_label_dsc_init(&txt_dsc); \
  txt_dsc.color = txt_clr; \
  txt_dsc.font = app.font##font_size; \
  txt_dsc.letter_space = 1; \
  txt_dsc.decor = LV_TEXT_DECOR_NONE; \
  txt_dsc.align = LV_TEXT_ALIGN_LEFT; \
  txt_dsc.text = str; \
  lv_draw_label(layer, &txt_dsc, &area); \
})

#define draw_text_right(str, font_size, _x, _y, bg_clr, txt_clr) ({ \
  /* Get the size of the text and create an area */ \
  lv_point_t size; \
  lv_text_get_size(&size, str, app.font##font_size, 1, 0, LVGL_CANVAS_WIDTH, LV_TEXT_FLAG_NONE); \
  lv_area_t area = {_x - size.x + 1, _y, _x, _y + size.y - 1}; \
  /* Draw a rectangle behind the text */ \
  lv_draw_rect_dsc_t rect_dsc; \
  lv_draw_rect_dsc_init(&rect_dsc); \
  rect_dsc.bg_color = bg_clr; \
  lv_draw_rect(layer, &rect_dsc, &area); \
  /* Draw the text */ \
  lv_draw_label_dsc_t txt_dsc; \
  lv_draw_label_dsc_init(&txt_dsc); \
  txt_dsc.color = txt_clr; \
  txt_dsc.font = app.font##font_size; \
  txt_dsc.letter_space = 1; \
  txt_dsc.decor = LV_TEXT_DECOR_NONE; \
  txt_dsc.align = LV_TEXT_ALIGN_RIGHT; \
  txt_dsc.text = str; \
  lv_draw_label(layer, &txt_dsc, &area); \
})

#define draw_text_center(str, font_size, _x, _y, bg_clr, txt_clr) ({ \
  /* Get the size of the text and create an area */ \
  lv_point_t size; \
  lv_text_get_size(&size, str, app.font##font_size, 1, 0, LVGL_CANVAS_WIDTH, LV_TEXT_FLAG_NONE); \
  lv_area_t area = {_x - size.x / 2 - 1, _y, _x + size.x / 2 + 1, _y + size.y - 1}; \
  /* Draw a rectangle behind the text */ \
  lv_draw_rect_dsc_t rect_dsc; \
  lv_draw_rect_dsc_init(&rect_dsc); \
  rect_dsc.bg_color = bg_clr; \
  lv_draw_rect(layer, &rect_dsc, &area); \
  /* Draw the text */ \
  lv_draw_label_dsc_t txt_dsc; \
  lv_draw_label_dsc_init(&txt_dsc); \
  txt_dsc.color = txt_clr; \
  txt_dsc.font = app.font##font_size; \
  txt_dsc.letter_space = 1; \
  txt_dsc.decor = LV_TEXT_DECOR_NONE; \
  txt_dsc.align = LV_TEXT_ALIGN_CENTER; \
  txt_dsc.text = str; \
  lv_draw_label(layer, &txt_dsc, &area); \
})

static inline lv_svg_node_t *load_svg(const char *svg_data)
{
  return lv_svg_load_data(svg_data, lv_strlen(svg_data));
}

/* Functions to get the matrix out of a lvgl svg */
nema_matrix3x3_t *get_svg_root_matrix(lv_svg_node_t *svg);
nema_matrix3x3_t *get_svg_child_matrix(lv_svg_node_t *svg, int idx);

/* Macro function for the draw_svg macro */
#define __assign_and_copy_matrix(idx, fun, ...) ({ \
  __matrixes[idx] = fun(__VA_ARGS__); \
  nema_mat3x3_copy(__original_matrixes[idx], *__matrixes[idx]); \
})

/* Macro function to make the drawing of svg easier to write */
#define draw_svg_transform(svg, num_of_child_group, callback) ({ \
  /* Initialize the temporary matrixes storage */ \
  nema_matrix3x3_t *__matrixes[1 + num_of_child_group]; \
  nema_matrix3x3_t __original_matrixes[1 + num_of_child_group]; \
  /* Add references to svg group matrixes and make a copy of them */ \
  __assign_and_copy_matrix(0, get_svg_root_matrix, svg); \
  for(int __idx = 1; __idx <= num_of_child_group; __idx++) { \
    __assign_and_copy_matrix(__idx, get_svg_child_matrix, svg, __idx - 1); \
  } \
  /* Execute the custom transformation subroutine and draw the svg to the layer */ \
  __attribute__((always_inline)) inline void __subroutine (nema_matrix3x3_t *__matrixes[1 + num_of_child_group]) callback; \
  __subroutine(__matrixes); \
  lv_draw_svg(layer, svg); \
  /* Copy back the original matrixes to the svg group matrixes */ \
  for(int __idx = 0; __idx <= num_of_child_group; __idx++) \
    nema_mat3x3_copy(*__matrixes[__idx], __original_matrixes[__idx]); \
})

/* All in one matrix transformation macro using NemaGFX */
#define transform_matrix(matrix, method, ...) ({ \
  nema_matrix3x3_t __transform_matrix; \
  nema_mat3x3_load_identity(__transform_matrix); \
  nema_mat3x3_##method(__transform_matrix, __VA_ARGS__); \
  nema_mat3x3_mul(*matrix, __transform_matrix); \
})

/*
 * NemaGFX matrix transformation methods:
 * translate(float tx, float ty)
 * scale(float sx, float sy)
 * shear(float shx, float shy)
 * mirror(float mx, float my)
 * rotate(float angle_degrees)
 * rotate2(float cosa, float sina)
 */

/* Shortcut macros to transform svg while drawing it */
#define transform_root(method, ...) transform_matrix(__matrixes[0], method, __VA_ARGS__)
#define transform_child(child_idx, method, ...) transform_matrix(__matrixes[1+child_idx], method, __VA_ARGS__)

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define IS_REPRESENTIBLE_IN_D_BITS(D, N)                \
  (((unsigned long) N >= (1UL << (D - 1)) && (unsigned long) N < (1UL << D)) ? D : -1)

#define BITS_TO_REPRESENT(N)                            \
  (N == 0 ? 1 : (31                                     \
                 + IS_REPRESENTIBLE_IN_D_BITS( 1, N)    \
                 + IS_REPRESENTIBLE_IN_D_BITS( 2, N)    \
                 + IS_REPRESENTIBLE_IN_D_BITS( 3, N)    \
                 + IS_REPRESENTIBLE_IN_D_BITS( 4, N)    \
                 + IS_REPRESENTIBLE_IN_D_BITS( 5, N)    \
                 + IS_REPRESENTIBLE_IN_D_BITS( 6, N)    \
                 + IS_REPRESENTIBLE_IN_D_BITS( 7, N)    \
                 + IS_REPRESENTIBLE_IN_D_BITS( 8, N)    \
                 + IS_REPRESENTIBLE_IN_D_BITS( 9, N)    \
                 + IS_REPRESENTIBLE_IN_D_BITS(10, N)    \
                 + IS_REPRESENTIBLE_IN_D_BITS(11, N)    \
                 + IS_REPRESENTIBLE_IN_D_BITS(12, N)    \
                 + IS_REPRESENTIBLE_IN_D_BITS(13, N)    \
                 + IS_REPRESENTIBLE_IN_D_BITS(14, N)    \
                 + IS_REPRESENTIBLE_IN_D_BITS(15, N)    \
                 + IS_REPRESENTIBLE_IN_D_BITS(16, N)    \
                 + IS_REPRESENTIBLE_IN_D_BITS(17, N)    \
                 + IS_REPRESENTIBLE_IN_D_BITS(18, N)    \
                 + IS_REPRESENTIBLE_IN_D_BITS(19, N)    \
                 + IS_REPRESENTIBLE_IN_D_BITS(20, N)    \
                 + IS_REPRESENTIBLE_IN_D_BITS(21, N)    \
                 + IS_REPRESENTIBLE_IN_D_BITS(22, N)    \
                 + IS_REPRESENTIBLE_IN_D_BITS(23, N)    \
                 + IS_REPRESENTIBLE_IN_D_BITS(24, N)    \
                 + IS_REPRESENTIBLE_IN_D_BITS(25, N)    \
                 + IS_REPRESENTIBLE_IN_D_BITS(26, N)    \
                 + IS_REPRESENTIBLE_IN_D_BITS(27, N)    \
                 + IS_REPRESENTIBLE_IN_D_BITS(28, N)    \
                 + IS_REPRESENTIBLE_IN_D_BITS(29, N)    \
                 + IS_REPRESENTIBLE_IN_D_BITS(30, N)    \
                 + IS_REPRESENTIBLE_IN_D_BITS(31, N)    \
                 + IS_REPRESENTIBLE_IN_D_BITS(32, N)    \
                 )                                      \
   )

#endif // HELPER_H
