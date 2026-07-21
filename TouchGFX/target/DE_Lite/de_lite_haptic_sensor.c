/**
  ******************************************************************************
  * @file    de_lite_haptic_sensor.c
  * @author  Anders Engineering Team
  * @version 1.0
  * @brief   This file provides a set of functions needed to control the haptic sensor
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 Anders Electronics plc.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */


/******************************************************************************
 * Change History Table
 * -----------------------------------------------------------------------------
 * Version | Date       | Author       | Description
 * -----------------------------------------------------------------------------
 * -----------------------------------------------------------------------------
 * 1.0     | 2026-03-28 | R.Gopan   | Initial BSP
 * -----------------------------------------------------------------------------
 * Notes:
 * - Always increment version when making manual changes.
 * - Describe clearly what was changed in each version.
 ******************************************************************************/

/* File Info : -----------------------------------------------------------------
                                   User NOTES
1. Driver description:
----------------------
   - This driver is used to control the haptic sensor

2. How To use this driver:
--------------------------
  + Initialization steps:
     o Make sure that the MX_GPIO_Init() function from the STM32 HAL layer is called.

  +  Haptic sensor on the use
     o Use BSP_HAPTIC_ON() to turn on the haptic feedback
     o Use BSP_HAPTIC_OFF() to turn off the haptic feedback
------------------------------------------------------------------------------*/

#include "de_lite_haptic_sensor.h"

#if TARGET_DE_LITE_DE_LITE_HAPTIC_SENSOR_H_ != 0100	/* BSP haptic sensor REVISION v.1.0 */
#error Wrong API version file (de_lite_haptic_sensor.h).
#endif


/**
  * @brief  Turn on the Haptic feedback
  */
void BSP_HAPTIC_ON(){
	HAL_GPIO_WritePin(Motor_Ctrl_GPIO_Port, Motor_Ctrl_Pin, GPIO_PIN_SET);
}


/**
  * @brief  Turn off the Haptic feedback
  */
void BSP_HAPTIC_OFF(){
	HAL_GPIO_WritePin(Motor_Ctrl_GPIO_Port, Motor_Ctrl_Pin, GPIO_PIN_RESET);
}
