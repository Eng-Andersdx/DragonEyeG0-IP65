/**
  ******************************************************************************
  * @file    gc9a01.h
  * @author  Anders Engineering Team
  * @version 1.0
  * @brief   This file contains definitions for the Display Driver IC GC9A10
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
 * 1.0     | 2026-03-30 | R.Gopan      | Initial header
 * -----------------------------------------------------------------------------
 * Notes:
 * - Always increment version when making manual changes.
 * - Describe clearly what was changed in each version.
 ******************************************************************************/

#ifndef TARGET_DE_LITE_COMPONENTS_GC9A01_H_
#define TARGET_DE_LITE_COMPONENTS_GC9A01_H_ 0100 /* BSP GC9A01 REVISION v.1.0 */

#include "main.h"

#define DISPLAY_COLUMN_ADDRESS_SET 0x2A
#define DISPLAY_ROW_ADDRESS_SET    0x2B
#define DISPLAY_ON                 0x29

void BSP_GC9A01_Init();
void BSP_GC9A01_Reset();


#endif /* TARGET_DE_LITE_COMPONENTS_GC9A01_H_ */
