/**
  ******************************************************************************
  * @file    de_lite_display.h
  * @author  Anders Engineering Team
  * @version 1.0
  * @brief   This file contains definitions for the T013240240-A2WMC-026 1.3" display
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

#ifndef BSP_DE_LITE_DE_LITE_DISPLAY_H_
#define BSP_DE_LITE_DE_LITE_DISPLAY_H_ 0100 /* BSP Display API REVISION v.1.0 */


#include "main.h"
#include "bsp.h"
#include "assert.h"
#include "Components/gc9a01.h"

#ifdef __cplusplus
extern "C" {
#endif

void 			Display_DCS_Send(uint8_t command);
uint32_t 			Display_DCS_Read(uint8_t address);
void 			Display_DCS_Send_With_Data(uint8_t command, uint8_t* data, uint16_t size);
void 					Display_Reset();
void 					DisplayDriver_Init(void);
void 			Display_DCS_Send_Data(uint8_t* data,uint16_t size);
void 			touchgfxDisplayDriverTransmitBlock(const uint8_t* pixels, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void 			touchgfxSignalVSync(void);
void            DisplayDriver_TransferCompleteCallback();
void 			DisplayDriverDMA_SPI_Init(void);
void 			startDmaTransfer(const uint8_t* data, uint32_t size);
void 			DisplayDriver_DMACallback();
void 			DataReader_DMACallback();

#ifdef __cplusplus
}
#endif /* BSP_DE_LITE_DE_LITE_DISPLAY_H_ */

#endif
