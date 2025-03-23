#include "app/usb.h"

uint8_t datas[USB_DATA_BUF_SIZE];

static void process_usb_data(uint32_t size)
{
  uint32_t pos = 0;
  while(pos < size)
  {
    usb_data_t *data = (usb_data_t *)(datas + pos);
    pos += 1;

    switch(data->data_type)
    {
      case POWER:
        app.data.power = data->data.u8 != 0;
        HAL_GPIO_WritePin(LCD_ON_GPIO_Port, LCD_ON_Pin, app.data.power);
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, LCD_BRIGHTNESS * app.data.power);
        pos += 1;
        break;
      case CARBURATOR_TEMP_LEVEL:
        app.data.carburator_temp_level = data->data.u8;
        pos += 1;
        break;
      case FLIGHT_TIME:
	app.data.flight_time = data->data.u64;
	pos += 8;
	break;
      case ENGINE_FIRE:
	app.data.engine_fire = data->data.u8 != 0;
	pos += 1;
	break;
      case CRASHED:
	app.data.crashed = data->data.u8 != 0;
	pos += 1;
	break;
      case PAUSED:
	app.data.paused = data->data.u8 != 0;
	pos += 1;
	break;
      default:
        float *values = &app.data.carburator_temp;
        values[data->data_type - USB_FLOAT_START] = data->data.f32;
        pos += 4;
        break;
    }
  }
}

static void cdc_task(void) {
  for (uint8_t itf = 0; itf < CFG_TUD_CDC; itf++) {
    if (tud_cdc_n_available(itf)) {
      uint32_t size = tud_cdc_n_read(itf, datas, sizeof(datas));

      process_usb_data(size);
    }
  }
}

void usb_init()
{
  HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, GPIO_PIN_SET);

  tud_init(BOARD_TUD_RHPORT);
}

void app_usb(void *argument)
{
  while (1)
  {
    tud_task();
    cdc_task();

    osDelay(1);
  }
}

void tud_mount_cb(void) {
  HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, GPIO_PIN_RESET);
}

void tud_resume_cb(void) {
  HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, GPIO_PIN_RESET);
}

void tud_umount_cb(void) {
  HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, GPIO_PIN_SET);
}

void tud_suspend_cb(bool remote_wakeup_en) {
  HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, GPIO_PIN_SET);
}
