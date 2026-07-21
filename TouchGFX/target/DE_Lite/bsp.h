/**
  ******************************************************************************
  * @file    bsp.h
  * @author  MCD Application Team
  * @brief   This file contains generic definitions for BSP framework and
  *          hardware resources.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */



/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_H
#define __BSP_H

#ifdef __cplusplus
 extern "C" {
#endif

#define DragonEyeG0
#define st1x33i_240x240

#define  GPIO_SPEED_HIGH                GPIO_SPEED_FREQ_VERY_HIGH
 /* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_ll_spi.h"
#include "stm32g0xx_ll_gpio.h"
#include "stm32g0xx_ll_tim.h"


/** @addtogroup BSP
  * @{
  */

/** @addtogroup BSP
  * @{
  */

/** @addtogroup BSP_LOW_LEVEL
  * @{
  */

/** @defgroup BSP_LOW_LEVEL_Exported_Types BSP LOW LEVEL Exported Types
 * @{
 */


/** @addtogroup Exported_types
  * @{
  */
typedef enum
{
  PB_SET = 0,
  PB_RESET = !PB_SET,
  PB_LONG_PRESS
} ButtonValue_TypeDef;


/** @brief BSP_Status_TypeDef
  *  BSP Status return possible values.
  */
typedef enum
{
 BSP_OK    = 0,
 BSP_ERROR = 1

} BSP_Status_TypeDef;

/**
  * @}
  */

/**
  * @}
  */

/** @defgroup BSP_LOW_LEVEL_Exported_Constants BSP LOW LEVEL Exported Constants
  * @{
  */



/**
  * @}
  */

/** @defgroup BSP_LOW_LEVEL_Exported_Macros BSP LOW LEVEL Exported Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup BSP_LOW_LEVEL_Exported_Functions BSP LOW LEVEL Exported Functions
  * @{
  */
uint32_t         BSP_GetVersion(void);


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */


#ifdef __cplusplus
}
#endif

#endif /* __BSP_H */
