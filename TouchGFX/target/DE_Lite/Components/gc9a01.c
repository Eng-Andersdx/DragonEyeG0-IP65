/**
  ******************************************************************************
  * @file    gc9a01.c
  * @author  Anders Engineering Team
  * @version 1.0
  * @brief   This file provides the initialisation function for the Display Driver IC GC9A01
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
 * 1.0     | 2026-03-30 | R.Gopan      | Initial Init Code
 * -----------------------------------------------------------------------------
 * Notes:
 * - Always increment version when making manual changes.
 * - Describe clearly what was changed in each version.
 ******************************************************************************/

/* File Info : -----------------------------------------------------------------
                                   User NOTES
1. Driver description:
----------------------
   - This driver is used to drive the display in the DragonEyeG0 development board.
     The driver uses DMA to send data to the display.
     The driver is also optimised for speed by using LL drivers.

2. How To use this driver:
--------------------------
  + Initialization steps:
     o Call the BSP_GC9A01_Reset() to reset the display
     o Call the BSP_GC9A01_Init() to initialise the display
------------------------------------------------------------------------------*/


#include "gc9a01.h"
#include "de_lite_display.h"


#if TARGET_DE_LITE_COMPONENTS_GC9A01_H_ != 0100	/* BSP GC9A01 REVISION v.1.0 */
#error Wrong API version file (gc9a01.h).
#endif

void BSP_GC9A01_Init(){
	Display_DCS_Send(0xFE);
	Display_DCS_Send(0xEF);
	Display_DCS_Send_With_Data(0xEB, (uint8_t[]){0x14}, 1);
	Display_DCS_Send_With_Data(0x84, (uint8_t[]){0x60}, 1);
	Display_DCS_Send_With_Data(0x85, (uint8_t[]){0xFF}, 1);
	Display_DCS_Send_With_Data(0x86, (uint8_t[]){0xFF}, 1);
	Display_DCS_Send_With_Data(0x87, (uint8_t[]){0xFF}, 1);
	Display_DCS_Send_With_Data(0x8E, (uint8_t[]){0xFF}, 1);
    Display_DCS_Send_With_Data(0x8F, (uint8_t[]){0xFF}, 1);
    Display_DCS_Send_With_Data(0x88, (uint8_t[]){0x0A}, 1);
    Display_DCS_Send_With_Data(0x89, (uint8_t[]){0x23}, 1);
    Display_DCS_Send_With_Data(0x8A, (uint8_t[]){0x00}, 1);
    Display_DCS_Send_With_Data(0x8B, (uint8_t[]){0x80}, 1);
    Display_DCS_Send_With_Data(0x8C, (uint8_t[]){0x01}, 1);
    Display_DCS_Send_With_Data(0x8D, (uint8_t[]){0x03}, 1);
    uint8_t data_b6[]={0x00,0x00};
    Display_DCS_Send_With_Data(0xB6, data_b6, 2);
    Display_DCS_Send_With_Data(0x36, (uint8_t[]){0x48}, 1);
    Display_DCS_Send_With_Data(0x3A, (uint8_t[]){0x05}, 1);
    uint8_t data_90[]={0x08,0x08,0x08,0x08};
    Display_DCS_Send_With_Data(0x90, data_90, 4);
    Display_DCS_Send_With_Data(0xBA, (uint8_t[]){0x0A}, 1);
    Display_DCS_Send_With_Data(0xBD, (uint8_t[]){0x06}, 1);
    Display_DCS_Send_With_Data(0xBC, (uint8_t[]){0x00}, 1);
    uint8_t data_ff[]={0x60,0x01,0x04};
    Display_DCS_Send_With_Data(0xff, data_ff, 3);
    Display_DCS_Send_With_Data(0xC3, (uint8_t[]){0x04}, 1);
    Display_DCS_Send_With_Data(0xC4, (uint8_t[]){0x04}, 1);
    Display_DCS_Send_With_Data(0xC9, (uint8_t[]){0x38}, 1);
    Display_DCS_Send_With_Data(0xBE, (uint8_t[]){0x11}, 1);
    uint8_t data_e1[]={0x10,0x0E};
    Display_DCS_Send_With_Data(0xE1, data_e1, 2);
    uint8_t data_df[]={0x21,0x0C,0x02};
    Display_DCS_Send_With_Data(0xDF, data_df, 3);
    uint8_t data_f0[]={0x4C,0x10,0x09,0x09,0x86,0x32};
    Display_DCS_Send_With_Data(0xF0, data_f0, 6);
    uint8_t data_f1[]={0x48,0x75,0x95,0x2E,0x34,0x8F};
    Display_DCS_Send_With_Data(0xF1, data_f1, 6);
    uint8_t data_f2[]={0x4C,0x10,0x09,0x09,0x86,0x32};
    Display_DCS_Send_With_Data(0xF2, data_f2, 6);
    uint8_t data_f3[]={0x48,0x75,0x95,0x2E,0x34,0x8F};
    Display_DCS_Send_With_Data(0xF3, data_f3, 6);
    uint8_t data_ed[]={0x1B,0x0B};
    Display_DCS_Send_With_Data(0xED, data_ed, 2);
    Display_DCS_Send_With_Data(0xAE, (uint8_t[]){0x77}, 1);
    Display_DCS_Send_With_Data(0xCD, (uint8_t[]){0x63}, 1);
    uint8_t data_70[]={0x07,0x07,0x04,0x0E,0x10,0x09,0x07,0x08,0x03};
    Display_DCS_Send_With_Data(0x70, data_70, 9);;
    Display_DCS_Send_With_Data(0xE8, (uint8_t[]){0x24}, 1);
    uint8_t data_62[]={0x18,0x0D,0x71,0xED,0x70,0x70,0x18,0x0F,0x71,0xEF,0x70,0x70};
    Display_DCS_Send_With_Data(0x62, data_62, 12);
    uint8_t data_63[]={0x18,0x11,0x71,0xF1,0x70,0x70,0x18,0x13,0x71,0xF3,0x70,0x70};
    Display_DCS_Send_With_Data(0x63, data_63, 12);
    uint8_t data_64[]={0x3B,0x29,0xF1,0x01,0xF1,0x00,0x0A};
    Display_DCS_Send_With_Data(0x64, data_64, 7);
    uint8_t data_66[]={0x3C,0x00,0xCD,0x67,0x45,0x45,0x10,0x00,0x00,0x00};
    Display_DCS_Send_With_Data(0x66, data_66, 10);
    uint8_t data_67[]={0x00,0x3C,0x00,0x00,0x00,0x01,0x54,0x10,0x32,0x98};
    Display_DCS_Send_With_Data(0x67, data_67, 10);
    uint8_t data_74[]={0x10,0xAB,0x80,0x00,0x00,0x4E,0x00};
    Display_DCS_Send_With_Data(0x74, data_74, 7);
    uint8_t data_98[]={0x3E,0x07};
    Display_DCS_Send_With_Data(0x98, data_98, 2);
    uint8_t data_99[]={0x3E,0x07};
    Display_DCS_Send_With_Data(0x99, data_99, 2);
    Display_DCS_Send(0x35);
    Display_DCS_Send(0x21);
    HAL_Delay(120);
    Display_DCS_Send(0x11);
    HAL_Delay(120);
    Display_DCS_Send(0x2C);
}

void BSP_GC9A01_Reset(){
	DISPLAY_RESET_GPIO_Port->BSRR = DISPLAY_RESET_Pin;
	HAL_Delay(50);
	DISPLAY_RESET_GPIO_Port->BRR = DISPLAY_RESET_Pin;
	HAL_Delay(50);
	DISPLAY_RESET_GPIO_Port->BSRR = DISPLAY_RESET_Pin;
	HAL_Delay(120);
}
