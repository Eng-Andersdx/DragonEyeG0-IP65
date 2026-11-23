/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_hal.h"

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

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define STMG0_485_DIR_Pin GPIO_PIN_13
#define STMG0_485_DIR_GPIO_Port GPIOC
#define HALL_S1_Pin GPIO_PIN_6
#define HALL_S1_GPIO_Port GPIOA
#define HALL_S1_EXTI_IRQn EXTI4_15_IRQn
#define HALL_S2_Pin GPIO_PIN_7
#define HALL_S2_GPIO_Port GPIOA
#define HALL_S2_EXTI_IRQn EXTI4_15_IRQn
#define DISPLAY_CSX_Pin GPIO_PIN_0
#define DISPLAY_CSX_GPIO_Port GPIOB
#define CTP_INT_Pin GPIO_PIN_1
#define CTP_INT_GPIO_Port GPIOB
#define CTP_INT_EXTI_IRQn EXTI0_1_IRQn
#define CTP_RST_Pin GPIO_PIN_2
#define CTP_RST_GPIO_Port GPIOB
#define FLASH_CSX_Pin GPIO_PIN_12
#define FLASH_CSX_GPIO_Port GPIOB
#define DSI_BL_CTRL_Pin GPIO_PIN_8
#define DSI_BL_CTRL_GPIO_Port GPIOA
#define B_LED_GPIO_Pin GPIO_PIN_9
#define B_LED_GPIO_GPIO_Port GPIOA
#define Motor_Ctrl_Pin GPIO_PIN_7
#define Motor_Ctrl_GPIO_Port GPIOC
#define G_LED_GPIO_Pin GPIO_PIN_10
#define G_LED_GPIO_GPIO_Port GPIOA
#define R_LED_GPIO_Pin GPIO_PIN_11
#define R_LED_GPIO_GPIO_Port GPIOA
#define DISPLAY_RESET_Pin GPIO_PIN_0
#define DISPLAY_RESET_GPIO_Port GPIOD
#define DISPLAY_TEARING_EFFECT_Pin GPIO_PIN_2
#define DISPLAY_TEARING_EFFECT_GPIO_Port GPIOD
#define DISPLAY_TEARING_EFFECT_EXTI_IRQn EXTI2_3_IRQn
#define DISPLAY_DCX_Pin GPIO_PIN_6
#define DISPLAY_DCX_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
