#include "libs/lvgl_display.h"

static void display_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);
#if FLUSH_METHOD == FLUSH_DMA2D
static void display_dma2d_initialize();
static void display_dma2d_configure_first_pass();
static void display_dma2d_configure_second_pass();
static void display_dma2d_blend_line();
static void display_dma2d_complete_irq();
#elif FLUSH_METHOD == FLUSH_NEMA
static void display_nema_flush();
static void display_nema_wait_flush(lv_display_t * disp);
#endif
static uint32_t get_flush_cf_from_layer_cf(uint32_t cf);
static uint32_t get_flush_cf_from_lv_cf(uint32_t cf);
static uint32_t get_stride_from_flush_cf(uint32_t size, uint32_t cf);

#if FLUSH_METHOD == FLUSH_DMA2D
static __attribute__((aligned(SCALE_BUFFER_COLOR_ALIGNMENT))) \
    uint8_t scale_buffer[LVGL_DISPLAY_WIDTH * LTDC_DISPLAY_HEIGHT * SCALE_BUFFER_COLOR_BYTE_SIZE];
#endif

// Display context for the flush process
display_context_t display = (display_context_t){
#if FLUSH_METHOD == FLUSH_DMA2D
  .dma2d = {
    .step = DISPLAY_STEP_IDLE,
    .scale = {
      .buf_color_format = SCALE_BUFFER_COLOR_FORMAT,
      .buf_address = scale_buffer,
    }
  }
#endif
};

lv_display_t *init_lvgl_display(uint32_t layer_idx)
{
  // Add the layer index to the display context
  display.layer_index = layer_idx;

  // Create the LVGL display
  lv_display_t *disp = display.lv_disp = lv_display_create(LVGL_DISPLAY_WIDTH, LVGL_DISPLAY_HEIGHT);
  // Add the flush function to LVGL backend
  lv_display_set_flush_cb(disp, display_flush_cb);

  // Create and set the LVGL framebuffer with direct mode rendering
  static __attribute__((aligned(LVGL_COLOR_ALIGNMENT))) \
    uint8_t lvgl_buffer[LVGL_DISPLAY_WIDTH * LVGL_DISPLAY_HEIGHT * LVGL_COLOR_BYTE_SIZE];

  lv_display_set_buffers(disp, lvgl_buffer, NULL, sizeof(lvgl_buffer), LV_DISPLAY_RENDER_MODE_DIRECT);

#if FLUSH_METHOD == FLUSH_DMA2D
  // Set the callback function for DMA2D transfer complete interrupt
  hdma2d.XferCpltCallback = display_dma2d_complete_irq;
#elif FLUSH_METHOD == FLUSH_NEMA
  // Add the flush waiting function to LVGL backend
  lv_display_set_flush_wait_cb(disp, display_nema_wait_flush);
  // Get the LVGL Nema GFX Command List or create a new one if it don't exist
  lv_draw_unit_t *unit = LV_GLOBAL_DEFAULT()->draw_info.unit_head;
  while(unit)
  {
    if(strcmp(unit->name, "NEMA_GFX") == 0) break;
    unit = unit->next;
  }
  if(unit)
    display.nema.p_cl = &((lv_draw_nema_gfx_unit_t*)unit)->cl;
  else
  {
    display.nema.cl = nema_cl_create();
    display.nema.p_cl = &display.nema.cl;
    nema_cl_bind_circular(display.nema.p_cl);
  }
#endif

  return disp;
}

static void display_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
#if FLUSH_METHOD == FLUSH_DMA2D
  // Error if the step is not in idle, should never happen
  if(unlikely(display.dma2d.step != DISPLAY_STEP_IDLE)) Error_Handler();
#endif

  // Get the LTDC layer color format
  LTDC_LayerCfgTypeDef * layer_cfg = &hltdc.LayerCfg[display.layer_index];
  uint32_t layer_cf = layer_cfg->PixelFormat;

  // Get the LVGL color format
  lv_color_format_t cf = lv_display_get_color_format(disp);

  // Get the area width and height
  int32_t area_width = lv_area_get_width(area);
  int32_t area_height = lv_area_get_height(area);

  // Configure the display source
  display.source.area_x = area->x1;
  display.source.area_y = area->y1;
  display.source.area_width = area_width;
  display.source.area_height = area_height;
  display.source.buf_width = LVGL_DISPLAY_WIDTH;
  display.source.buf_height = LVGL_DISPLAY_HEIGHT;
  display.source.buf_color_format = get_flush_cf_from_lv_cf(cf);
#if FLUSH_METHOD == FLUSH_DMA2D
  display.source.buf_address = (void*)px_map +
			       get_stride_from_flush_cf(display.source.area_y * LVGL_DISPLAY_WIDTH +
							display.source.area_x,
							display.source.buf_color_format);
#elif FLUSH_METHOD == FLUSH_NEMA
  display.source.buf_address = (void*)px_map;
#endif

  // Configure the display destination
  display.destination.area_x = RATIO_WIDTH(area->x1);
  display.destination.area_y = RATIO_HEIGHT(area->y1);
  display.destination.area_width = RATIO_WIDTH(area_width);
  display.destination.area_height = RATIO_HEIGHT(area_height);
  display.destination.buf_width = LTDC_DISPLAY_WIDTH;
  display.destination.buf_height = LTDC_DISPLAY_HEIGHT;
  display.destination.buf_color_format = get_flush_cf_from_layer_cf(layer_cf);
#if FLUSH_METHOD == FLUSH_DMA2D
  display.destination.buf_address = (void*)ltdc_framebuffer +
				    get_stride_from_flush_cf(display.destination.area_y * LTDC_DISPLAY_WIDTH +
							     display.destination.area_x,
							     display.destination.buf_color_format);
#elif FLUSH_METHOD == FLUSH_NEMA
  display.destination.buf_address = (void*)ltdc_framebuffer;
#endif

#if FLUSH_METHOD == FLUSH_DMA2D
  // Initialize the DMA2D
  display_dma2d_initialize();

  // Configure it for the first pass
  display_dma2d_configure_first_pass();

  // Blend one line and start the DMA2D process
  display_dma2d_blend_line();
#elif FLUSH_METHOD == FLUSH_NEMA
  // Flush using NemaGFX
  display_nema_flush();
#endif
}

#if FLUSH_METHOD == FLUSH_DMA2D
static void display_dma2d_initialize()
{
  // Waiting the DMA2D to stop
  while(DMA2D->CR & DMA2D_CR_START_Msk);

  // MODE: Memory-to-Memory with blending (FG & BG)
  DMA2D->CR = DMA2D_M2M_BLEND;
}

static void display_dma2d_configure_first_pass()
{
  // Change display context step to first pass
  display.dma2d.step = DISPLAY_STEP_FIRST_PASS;

  // Configure the loop context for the first pass
  display.dma2d.loop.counter            = display.destination.area_height;
  display.dma2d.loop.source_stride      = get_stride_from_flush_cf(display.source.buf_width, display.source.buf_color_format);
  display.dma2d.loop.destination_stride = get_stride_from_flush_cf(display.source.area_width, display.dma2d.scale.buf_color_format);
  display.dma2d.loop.blend_coeff        = ((display.source.area_height - 1) << BLEND_SHIFT) / display.destination.area_height;
  display.dma2d.loop.blend_index        = display.dma2d.loop.blend_coeff >> 1;
  display.dma2d.loop.base_address       = display.source.buf_address;

  // Configure the format, the alpha mode and the initial alpha value of the foreground image
  DMA2D->FGPFCCR = (DMA2D_REPLACE_ALPHA             << DMA2D_FGPFCCR_AM_Pos    |
		            display.source.buf_color_format << DMA2D_FGPFCCR_CM_Pos    |
		            0x00                            << DMA2D_FGPFCCR_ALPHA_Pos );

  // Configure the format, the alpha mode and the initial alpha value of the background image
  DMA2D->BGPFCCR = (DMA2D_REPLACE_ALPHA             << DMA2D_BGPFCCR_AM_Pos    |
		            display.source.buf_color_format << DMA2D_BGPFCCR_CM_Pos    |
		            0xFF                            << DMA2D_BGPFCCR_ALPHA_Pos );

  // Configure the color format and the buffer address of the output image
  DMA2D->OPFCCR  = (display.dma2d.scale.buf_color_format << DMA2D_OPFCCR_CM_Pos);
  DMA2D->OMAR    = (uint32_t)display.dma2d.scale.buf_address;

  // Configure the DMA2D to process 1 line and source width column
  DMA2D->NLR     = (1                        << DMA2D_NLR_NL_Pos |
		   display.source.area_width << DMA2D_NLR_PL_Pos );
}

static void display_dma2d_configure_second_pass()
{
  // Change display context step to second pass
  display.dma2d.step = DISPLAY_STEP_SECOND_PASS;

  // Configure the loop context for the second pass
  display.dma2d.loop.counter            = display.destination.area_width;
  display.dma2d.loop.source_stride      = get_stride_from_flush_cf(1, display.dma2d.scale.buf_color_format);
  display.dma2d.loop.destination_stride = get_stride_from_flush_cf(1, display.destination.buf_color_format);
  display.dma2d.loop.blend_coeff        = ((display.source.area_width - 1) << BLEND_SHIFT) / display.destination.area_width;
  display.dma2d.loop.blend_index        = display.dma2d.loop.blend_coeff >> 1;
  display.dma2d.loop.base_address       = display.dma2d.scale.buf_address;

  // Configure the format, the alpha mode and the initial alpha value of the foreground image
  DMA2D->FGPFCCR = (DMA2D_REPLACE_ALPHA                  << DMA2D_FGPFCCR_AM_Pos    |
		            display.dma2d.scale.buf_color_format << DMA2D_FGPFCCR_CM_Pos    |
		            0x00                                 << DMA2D_FGPFCCR_ALPHA_Pos );

  // Configure the DMA2D to skip (source width - 1) bytes at each line in the foreground
  DMA2D->FGOR    = ((display.source.area_width - 1) << DMA2D_FGOR_LO_Pos);

  // Configure the format, the alpha mode and the initial alpha value of the background image
  DMA2D->BGPFCCR = (DMA2D_REPLACE_ALPHA                  << DMA2D_BGPFCCR_AM_Pos    |
		            display.dma2d.scale.buf_color_format << DMA2D_BGPFCCR_CM_Pos    |
		            0xFF                                 << DMA2D_BGPFCCR_ALPHA_Pos );

  // Configure the DMA2D to skip (source width - 1) bytes at each line in the background
  DMA2D->BGOR    = ((display.source.area_width - 1) << DMA2D_FGOR_LO_Pos);

  // Configure the color format and the buffer address of the output image
  DMA2D->OPFCCR  = (display.destination.buf_color_format << DMA2D_OPFCCR_CM_Pos);
  DMA2D->OMAR    = (uint32_t)display.destination.buf_address;

  // Configure the DMA2D to skip (ltdc display width - 1) bytes at each line in the output
  DMA2D->OOR     = ((display.destination.buf_width - 1) << DMA2D_OOR_LO_Pos);

  // Configure the DMA2D to process destination height line and 1 column
  DMA2D->NLR     = (display.destination.area_height << DMA2D_NLR_NL_Pos |
		    1                               << DMA2D_NLR_PL_Pos );
}

static void display_dma2d_blend_line()
{
  // Compute the line to process and the alpha balance for the blending
  uint32_t first_line       = display.dma2d.loop.blend_index >>  BLEND_SHIFT;
  uint32_t foreground_alpha = display.dma2d.loop.blend_index >> (BLEND_SHIFT - 8);

  // Compute the start address of the background and foreground image
  uint32_t address = (uint32_t)display.dma2d.loop.base_address + first_line * display.dma2d.loop.source_stride;
  DMA2D->BGMAR = address;
  DMA2D->FGMAR = address + display.dma2d.loop.source_stride;

  // Set the new alpha value to the foreground
  DMA2D->FGPFCCR &= ~DMA2D_FGPFCCR_ALPHA_Msk;
  DMA2D->FGPFCCR |= (foreground_alpha << DMA2D_FGPFCCR_ALPHA_Pos);

  // Ensure the DMA2D register values are observed before the start transfer bit is set
  __DSB();

  // Start the DMA2D
  DMA2D->CR |= (DMA2D_CR_START |  // START DMA2D
		DMA2D_IT_TC    ); // ENABLE Transfer Complete Interrupt
}

static void display_dma2d_complete_irq()
{
  if(likely(display.dma2d.step != DISPLAY_STEP_IDLE))
  {
    if(--display.dma2d.loop.counter)
    {
      // At each loop, till the end of the counter, add the destination stride to the output address
      DMA2D->OMAR += display.dma2d.loop.destination_stride;
      // And add the blend coeff to the blend index
      display.dma2d.loop.blend_index += display.dma2d.loop.blend_coeff;
      // Then blend the next line or column
      return display_dma2d_blend_line();
    }
    else if(display.dma2d.step == DISPLAY_STEP_FIRST_PASS)
    {
      // At the end of the first pass, configure the second pass
      display_dma2d_configure_second_pass();
      // And start the process by blending the first column
      return display_dma2d_blend_line();
    }
    else
    {
      // At the end of the second pass, set the step to idle
      display.dma2d.step = DISPLAY_STEP_IDLE;
      // And tell LVGL that it can flush again
      return lv_display_flush_ready(display.lv_disp);
    }
  }
}
#elif FLUSH_METHOD == FLUSH_NEMA
static void display_nema_flush()
{
  // Set the destination texture
  nema_bind_dst_tex((uintptr_t)display.destination.buf_address,
    display.destination.buf_width,
    display.destination.buf_height,
    display.destination.buf_color_format,
    get_stride_from_flush_cf(display.destination.buf_width, display.destination.buf_color_format)
  );

  // Set the source texture with bilinear filtering
  nema_bind_src_tex((uintptr_t)display.source.buf_address,
    display.source.buf_width,
    display.source.buf_height,
    display.source.buf_color_format,
    get_stride_from_flush_cf(display.source.buf_width, display.source.buf_color_format),
    NEMA_FILTER_BL
  );

  // Set the clipping area to the whole destination buffer
  nema_set_clip(
    0,
    0,
    display.destination.buf_width,
    display.destination.buf_height
  );

  // Set blending compute mode
  nema_set_blend_blit(NEMA_BL_SRC_OVER);

  // Copy the source area to the destination area
  nema_blit_subrect_fit(
    display.destination.area_x,
    display.destination.area_y,
    display.destination.area_width,
    display.destination.area_height,
    display.source.area_x,
    display.source.area_y,
    display.source.area_width,
    display.source.area_height
  );

  // Submit the commands to the GPU
  nema_cl_submit(display.nema.p_cl);

  // Store the current command list submission ID
  display.nema.submission_id = display.nema.p_cl->submission_id;
}

static void display_nema_wait_flush(lv_display_t * disp)
{
  // Read the GPU last command list id
  uint32_t last_cl_id = HAL_GPU2D_ReadRegister(&hgpu2d, GPU2D_CLID);
  // Compare it with the flush process submission id to see if it as finished
  if(last_cl_id >= display.nema.submission_id){
    // Tell LVGL that it can flush again
    lv_display_flush_ready(disp);
  }
}
#endif

static uint32_t get_flush_cf_from_layer_cf(uint32_t cf)
{
#if FLUSH_METHOD == FLUSH_DMA2D
  switch(cf) {
    case LTDC_PIXEL_FORMAT_ARGB8888:
      return DMA2D_INPUT_ARGB8888;
    case LTDC_PIXEL_FORMAT_RGB888:
      return DMA2D_INPUT_RGB888;
    case LTDC_PIXEL_FORMAT_RGB565:
      return DMA2D_INPUT_RGB565;
    case LTDC_PIXEL_FORMAT_ARGB1555:
      return DMA2D_INPUT_ARGB1555;
    case LTDC_PIXEL_FORMAT_ARGB4444:
      return DMA2D_INPUT_ARGB4444;
  }
#elif FLUSH_METHOD == FLUSH_NEMA
  switch(cf) {
    case LTDC_PIXEL_FORMAT_ARGB8888:
      return NEMA_ARGB8888;
    case LTDC_PIXEL_FORMAT_RGB565:
      return NEMA_RGB565;
    case LTDC_PIXEL_FORMAT_ARGB1555:
      return NEMA_ARGB1555;
    case LTDC_PIXEL_FORMAT_ARGB4444:
      return NEMA_ARGB4444;
  }
#endif
  Error_Handler();
  return -1;
}

static uint32_t get_flush_cf_from_lv_cf(uint32_t cf)
{
#if FLUSH_METHOD == FLUSH_DMA2D
  switch(cf) {
    case LV_COLOR_FORMAT_ARGB8888:
      return DMA2D_INPUT_ARGB8888;
    case LV_COLOR_FORMAT_RGB888:
      return DMA2D_INPUT_RGB888;
    case LV_COLOR_FORMAT_RGB565:
      return DMA2D_INPUT_RGB565;
    case LV_COLOR_FORMAT_ARGB1555:
      return DMA2D_INPUT_ARGB1555;
    case LV_COLOR_FORMAT_ARGB4444:
      return DMA2D_INPUT_ARGB4444;
  }
#elif FLUSH_METHOD == FLUSH_NEMA
  switch(cf) {
    case LV_COLOR_FORMAT_ARGB8888:
      return NEMA_ARGB8888;
    case LV_COLOR_FORMAT_XRGB8888:
      return NEMA_XRGB8888;
    case LV_COLOR_FORMAT_ARGB4444:
      return NEMA_ARGB4444;
    case LV_COLOR_FORMAT_ARGB1555:
      return NEMA_ARGB1555;
    case LV_COLOR_FORMAT_ARGB2222:
      return NEMA_ARGB2222;
    case LV_COLOR_FORMAT_RGB565:
      return NEMA_RGB565;
    case LV_COLOR_FORMAT_NEMA_TSC4:
      return NEMA_TSC4;
    case LV_COLOR_FORMAT_NEMA_TSC6:
      return NEMA_TSC6;
    case LV_COLOR_FORMAT_NEMA_TSC6A:
      return NEMA_TSC6A;
    case LV_COLOR_FORMAT_NEMA_TSC6AP:
      return NEMA_TSC6AP;
    case LV_COLOR_FORMAT_NEMA_TSC12:
      return NEMA_TSC12;
    case LV_COLOR_FORMAT_NEMA_TSC12A:
      return NEMA_TSC12A;
  }
#endif
  Error_Handler();
  return -1;
}

static uint32_t get_stride_from_flush_cf(uint32_t size, uint32_t cf)
{
#if FLUSH_METHOD == FLUSH_DMA2D
  switch(cf) {
    case DMA2D_INPUT_ARGB8888:
      return size * 4;
    case DMA2D_INPUT_RGB888:
      return size * 3;
    case DMA2D_INPUT_RGB565:
    case DMA2D_INPUT_ARGB1555:
    case DMA2D_INPUT_ARGB4444:
      return size * 2;
  }
#elif FLUSH_METHOD == FLUSH_NEMA
  switch(cf) {
    case NEMA_ARGB8888:
    case NEMA_XRGB8888:
      return size * 4;
    case NEMA_ARGB4444:
    case NEMA_ARGB1555:
    case NEMA_RGB565:
      return size * 2;
    case NEMA_ARGB2222:
      return size;
    case NEMA_TSC4:
    case NEMA_TSC6:
    case NEMA_TSC6A:
    case NEMA_TSC6AP:
    case NEMA_TSC12:
    case NEMA_TSC12A:
      // Stride is computed by the GPU for NEMA_TSC images
      return -1;
  }
#endif
  Error_Handler();
  return -1;
}
