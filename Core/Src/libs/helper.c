#include "libs/helper.h"

nema_matrix3x3_t *get_svg_root_matrix(lv_svg_node_t *svg)
{
  lv_svg_node_t *node = (lv_svg_node_t *)svg->base.children[0];
  lv_svg_attr_t *attr = lv_array_at(&node->attrs, 0);
  lv_svg_matrix_t *matrix = attr->value.val;
  return (nema_matrix3x3_t*)&matrix->m;
}

nema_matrix3x3_t *get_svg_child_matrix(lv_svg_node_t *svg, int idx)
{
  lv_svg_node_t *node = (lv_svg_node_t *)svg->base.children[0]->children[idx];
  lv_svg_attr_t *attr = lv_array_at(&node->attrs, 0);
  lv_svg_matrix_t *matrix = attr->value.val;
  return (nema_matrix3x3_t*)&matrix->m;
}

float get_angle_ratio_from_value(float value, const float (*scale_values)[2], size_t len)
{
  if(value <= scale_values[0][0]) return scale_values[0][1];
  else if(value >= scale_values[len-1][0]) return scale_values[len-1][1];

  float angle_ratio = 0.0f;
  for(int i = 0; i < len; i++)
  {
    if(i+1 < len && value >= scale_values[i+1][0]) continue;
    angle_ratio = (((value - scale_values[i][0])/(scale_values[i+1][0] - scale_values[i][0])) *
	          (scale_values[i+1][1] - scale_values[i][1]) + scale_values[i][1]);
    break;
  }
  return angle_ratio;
}
