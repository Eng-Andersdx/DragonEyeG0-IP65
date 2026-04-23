/**
  ******************************************************************************
  * @file    de_lite_display.c
  * @author  Anders Engineering Team
  * @version 1.0
  * @brief   This file provides a set of functions needed to drive the T013240240-A2WMC-026 1.3" display
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
 * 1.0     | 2025-09-20 | R.Gopan      | Initial display driver
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
     o Reset the display using the Display_Reset() function.
       Enable the SPI and DMA using DisplayDriverDMA_SPI_Init() function.
       Initialise the display using DisplayDriver_Init() function.
       The initialisation is particular of a specific panel.
	   These functions performs hardware resources initialization & display initialisation.

  +  How to send data pixels to the display
     o Set the area in the display where the pixels should be updated using the function Display_Set_Area(uint16_t a_start, uint16_t a_end,
                      uint16_t b_start, uint16_t b_end);.
     o Use the startDmaTransfer() function to send the pixel data using dma.

------------------------------------------------------------------------------*/
#include <de_lite_display.h>

#if BSP_DE_LITE_DE_LITE_DISPLAY_H_ != 0100	/* BSP LEDS REVISION v.1.0 */
#error Wrong API version file (de_lite_display.h).
#endif






extern SPI_HandleTypeDef hspi1;
extern void MX_SPI1_Init(void);
extern DMA_HandleTypeDef hdma_spi1_tx;

volatile uint8_t IsTransmittingBlock = 0;
volatile uint16_t tearingEffectCount = 0;
static SPI_TypeDef* const DISPLAY_SPI = SPI1;


/**
  * @brief  Send Commands to the SPI Peripheral
  * @param  command:The command to be send to the peripheral
  */
void Display_DCS_Send(uint8_t command)
{
   // Reset the nCS pin
   LL_GPIO_ResetOutputPin(DISPLAY_CSX_GPIO_Port, DISPLAY_CSX_Pin);
   // Reset the DCX pin
   LL_GPIO_ResetOutputPin(DISPLAY_DCX_GPIO_Port, DISPLAY_DCX_Pin);

  //Send the command
  LL_SPI_TransmitData8(DISPLAY_SPI, command);

  // Wait until the bus is not busy before changing configuration
  while(LL_SPI_IsActiveFlag_BSY(DISPLAY_SPI));
  // Set the nCS
  LL_GPIO_SetOutputPin(DISPLAY_CSX_GPIO_Port, DISPLAY_CSX_Pin);
}


/**
  * @brief  Send both Commmand and Data to the SPI Peripheral using LL Driver
  * @param  command:The command to be send to the peripheral
  * @param  data:The data to be send to the peripheral
  * @param  size:The size (in bytes)
  */
void Display_DCS_Send_With_Data(uint8_t command, uint8_t* data, uint16_t size)
{
  // Reset the nCS pin
  LL_GPIO_ResetOutputPin(DISPLAY_CSX_GPIO_Port, DISPLAY_CSX_Pin);
  // Reset the DCX pin
  LL_GPIO_ResetOutputPin(DISPLAY_DCX_GPIO_Port, DISPLAY_DCX_Pin);

  //Send the command
  LL_SPI_TransmitData8(DISPLAY_SPI, command);

  // Wait until the bus is not busy before changing configuration
  while(LL_SPI_IsActiveFlag_BSY(DISPLAY_SPI));
  // Set the DCX pin
  LL_GPIO_SetOutputPin(DISPLAY_DCX_GPIO_Port, DISPLAY_DCX_Pin);
  //Send the data
  while (size > 0U)
    {
  	while (!LL_SPI_IsActiveFlag_TXE(DISPLAY_SPI));
  	LL_SPI_TransmitData8(DISPLAY_SPI, *data++);
    size--;
    }

  // Wait until the bus is not busy before changing configuration
  while(LL_SPI_IsActiveFlag_BSY(DISPLAY_SPI));
  // Set the nCS
  LL_GPIO_SetOutputPin(DISPLAY_CSX_GPIO_Port, DISPLAY_CSX_Pin);
}



/**
  * @brief  Reset the Display
**/
void Display_Reset(){
	BSP_GC9A01_Reset();
}

/**
 * @brief Set the area to be displayed
 * @param a_start:x axis start point
 * @param a_end:  x axis end point
 * @param b_start:y axis start point
 * @param b_end:  y axis end point
 * a_start and a_end point are responsible for the start and end point in x direction to be active for the display
 * b_start and b_end point are responsible for the start and end point in y direction to be active for the display
 */
void Display_Set_Area(uint16_t a_start, uint16_t a_end,
                      uint16_t b_start, uint16_t b_end)
{
  static uint16_t old_x0 = 0xFFFF;
  static uint16_t old_x1 = 0xFFFF;
  static uint16_t old_y0 = 0xFFFF;
  static uint16_t old_y1 = 0xFFFF;
  uint8_t arguments[4];

  if (a_start != old_x0 || b_start != old_x1){
    arguments[0] = a_start >> 8;
    arguments[1] = a_start & 0xFF;
    arguments[2] = a_end >> 8;
    arguments[3] = a_end & 0xFF;
    while (((hspi1.Instance->SR & SPI_FLAG_TXE) == 0) || ((hspi1.Instance->SR & SPI_FLAG_BSY) != 0));
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    HAL_SPI_Init(&hspi1);
    hspi1.Instance->CR2 = hspi1.Instance->CR2 | ( SPI_DATASIZE_8BIT & SPI_CR2_DS_Msk);
    LL_SPI_Enable(DISPLAY_SPI);
    LL_SPI_EnableDMAReq_TX(DISPLAY_SPI);
    Display_DCS_Send_With_Data(DISPLAY_COLUMN_ADDRESS_SET, arguments, 4);
    old_x0 = a_start;
    old_x1 = b_start;
  }

  if (a_end != old_y0 || b_end != old_y1){
    arguments[0] = b_start >> 8;
    arguments[1] = b_start & 0xFF;
    arguments[2] = b_end >> 8;
    arguments[3] = b_end & 0xFF;
    Display_DCS_Send_With_Data(DISPLAY_ROW_ADDRESS_SET, arguments, 4);
    old_y0 = a_end;
    old_y1 = b_end;
  }
}

/**
 * @brief Returns the variable IsTransmittingBlock_ which is responsible for telling the transmission status to the display
 * @return returns an integer type variable
 */
int touchgfxDisplayDriverTransmitActive(void)
{
  return IsTransmittingBlock;
}

/**
 * @brief Set the area of the display to be updated, send the respective pixels in partial blocks and start the transfer for the remaining blocks of framebuffer.
 * @param uin8_t* pixels: A pointer which points to the pixels from the framebuffer.
 * @param uint16_t x: Start point of the Display Active area in x direction
 * @param uint16_t y: Start point of the Display Active area in y direction
 * @param uint16_t w: Width of the Display Active area to be updated
 * @param uint16_t h: Height of the Display Active area to be updated
 */
void touchgfxDisplayDriverTransmitBlock(const uint8_t* pixels, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
	IsTransmittingBlock = 1;

	Display_Set_Area(x, x+w-1, y, y+h-1);
	LL_GPIO_ResetOutputPin(DISPLAY_CSX_GPIO_Port, DISPLAY_CSX_Pin);
	// Reset the DCX pin
	LL_GPIO_ResetOutputPin(DISPLAY_DCX_GPIO_Port, DISPLAY_DCX_Pin);
	//Send the command
	LL_SPI_TransmitData8(DISPLAY_SPI, 0x2C);
	while(LL_SPI_IsActiveFlag_BSY(DISPLAY_SPI));
	LL_GPIO_SetOutputPin(DISPLAY_DCX_GPIO_Port, DISPLAY_DCX_Pin); //indicate data transfer
	startDmaTransfer((uint8_t*)pixels, w * h * 2);

}


/**
 * @brief Initialisation code for the Display
 */
void DisplayDriver_Init(void){
	BSP_GC9A01_Init();
}



/**
 * @brief Initialisation code for the Display SPI and DMA
 */
void DisplayDriverDMA_SPI_Init(void)
{
  LL_SPI_Enable(DISPLAY_SPI);
  LL_SPI_EnableDMAReq_TX(DISPLAY_SPI);

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}

/**
  * @brief  Send Data to the SPI Peripheral using dma
  * @param  data:The data to be send to the peripheral
  * @param  size:The size (in bytes)
  */
void startDmaTransfer(const uint8_t* data, uint32_t size)
{
  __HAL_DMA_DISABLE(&hdma_spi1_tx);

  // Set the SPI in 16-bit mode to match endianess
  LL_SPI_SetDataWidth(DISPLAY_SPI, LL_SPI_DATAWIDTH_16BIT);


  /* Configure DMA Channel destination address */
  hdma_spi1_tx.Instance->CPAR = LL_SPI_DMA_GetRegAddr(DISPLAY_SPI);

  /* Configure DMA Channel source address */
  hdma_spi1_tx.Instance->CMAR = (uint32_t)data;

  /* Configure DMA Channel data length */
  hdma_spi1_tx.Instance->CNDTR = size;

  /* Disable the transfer half complete interrupt */
  __HAL_DMA_DISABLE_IT(&hdma_spi1_tx, DMA_IT_HT);
  /* Enable the transfer complete interrupt */
  __HAL_DMA_ENABLE_IT(&hdma_spi1_tx, (DMA_IT_TC | DMA_IT_TE));

  __HAL_DMA_ENABLE(&hdma_spi1_tx);
}

/**
 * @brief Callback Function for Display
 */
void DisplayDriver_DMACallback(void)
{
  /* Transfer Complete Interrupt management ***********************************/
  if ((0U != (DMA1->ISR & (DMA_FLAG_TC1))) && (0U != (hdma_spi1_tx.Instance->CCR & DMA_IT_TC)))
  {
    /* Disable the transfer complete and error interrupt */
    __HAL_DMA_DISABLE_IT(&hdma_spi1_tx, DMA_IT_TE | DMA_IT_TC);

    /* Clear the transfer complete flag */
    __HAL_DMA_CLEAR_FLAG(&hdma_spi1_tx, DMA_FLAG_TC1);

    IsTransmittingBlock =0;

    // Wait until the bus is not busy before changing configuration
    // SPI is busy in communication or Tx buffer is not empty
    while(LL_SPI_IsActiveFlag_BSY(DISPLAY_SPI));

    LL_GPIO_SetOutputPin(DISPLAY_CSX_GPIO_Port, DISPLAY_CSX_Pin);  //de-activate display transfer

    // Go back to 8-bit mode
    LL_SPI_SetDataWidth(DISPLAY_SPI, LL_SPI_DATAWIDTH_8BIT);

    // Signal Transfer Complete to TouchGFX
    DisplayDriver_TransferCompleteCallback();
    
    
  }
    /* Transfer Error Interrupt management **************************************/
  else if ((0U != (DMA1->ISR & (DMA_FLAG_TC1))) && (0U != (hdma_spi1_tx.Instance->CCR & DMA_IT_TE)))
  {
    /* When a DMA transfer error occurs */
    /* A hardware clear of its EN bits is performed */
    /* Disable ALL DMA IT */
    __HAL_DMA_DISABLE_IT(&hdma_spi1_tx, (DMA_IT_TC | DMA_IT_HT | DMA_IT_TE));

    /* Clear all flags */
    __HAL_DMA_CLEAR_FLAG(&hdma_spi1_tx, DMA_FLAG_GI1 );

    assert(0);  // Halting program - Transfer Error Interrupt received.
  }
}
