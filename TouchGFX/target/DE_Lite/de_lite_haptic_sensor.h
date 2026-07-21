/**
  ******************************************************************************
  * @file    de_lite_haptic_sensor.h
  * @author  Anders Engineering Team
  * @version 1.0
  * @brief   This file contains definitions for the haptic sensor
  *
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
 * 1.0     | 2026-03-27 | R.Gopan      | Initial header
 * -----------------------------------------------------------------------------
 * Notes:
 * - Always increment version when making manual changes.
 * - Describe clearly what was changed in each version.
 ******************************************************************************/


#ifndef TARGET_DE_LITE_DE_LITE_HAPTIC_SENSOR_H_
#define TARGET_DE_LITE_DE_LITE_HAPTIC_SENSOR_H_ 0100 /* BSP haptic sensor REVISION v.1.0 */

#ifdef __cplusplus
	extern "C" {
#endif

#include "main.h"

void BSP_HAPTIC_ON(void);
void BSP_HAPTIC_OFF(void);

#ifdef __cplusplus
}
#endif


#endif /* TARGET_DE_LITE_DE_LITE_HAPTIC_SENSOR_H_ */
