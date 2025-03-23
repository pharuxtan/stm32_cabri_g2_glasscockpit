/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#ifndef LVGL_CONF
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
// Attribute to write static data to external flash
#define EXT_FLASH __attribute__((section("ExtFlashSection"))) __attribute__((aligned(4)))
// Give hint to compiler and help the CPU Branch Prediction
#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
#ifdef __cplusplus
}
#endif

#endif // __MAIN_H

#endif

#ifndef __MAIN_H_DEF
#define __MAIN_H_DEF

#ifdef __cplusplus
extern "C" {
#endif
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LVGL_DISPLAY_HEIGHT 300
#define LTDC_DISPLAY_WIDTH 800
#define LTDC_DISPLAY_HEIGHT 480
#define LVGL_DISPLAY_WIDTH 500
#define LCD_ON_Pin GPIO_PIN_4
#define LCD_ON_GPIO_Port GPIOE
#define BL_CTRL_Pin GPIO_PIN_6
#define BL_CTRL_GPIO_Port GPIOE
#define USER_BUTTON_Pin GPIO_PIN_13
#define USER_BUTTON_GPIO_Port GPIOC
#define RED_LED_Pin GPIO_PIN_2
#define RED_LED_GPIO_Port GPIOD
#define GREEN_LED_Pin GPIO_PIN_4
#define GREEN_LED_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */
#ifdef __cplusplus
}
#endif

#endif // __MAIN_H_DEF

#ifndef LVGL_CONF

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MAIN_H_EXTRA
#define __MAIN_H_EXTRA

extern TIM_HandleTypeDef htim3;
extern LTDC_HandleTypeDef hltdc;
extern DMA2D_HandleTypeDef hdma2d;
extern GPU2D_HandleTypeDef hgpu2d;
extern volatile __attribute__((aligned(16))) uint8_t ltdc_framebuffer[LTDC_DISPLAY_WIDTH * LTDC_DISPLAY_HEIGHT * 2];

#endif // __MAIN_H_EXTRA

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
