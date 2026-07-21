/**
  ******************************************************************************
  * @file    MX25Lxx.c
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
 * 1.0     | 2025-09-20 | R.Gopan   | Initial BSP
 * -----------------------------------------------------------------------------
 * Notes:
 * - Always increment version when making manual changes.
 * - Describe clearly what was changed in each version.
 ******************************************************************************/

/* File Info : -----------------------------------------------------------------
                                   User NOTES
1. Driver description:
----------------------
   - This driver is used to read data from the external flash MX25L6433F in normal and DMA mode

2. How To use this driver:
--------------------------
  + Initialization steps:
     o Make sure that the MX_SPIx_Init(); function from the STM32 HAL layer is called.

  +  External Flash on the use
     o Use readData (uint32_t address24, uint8_t* buffer, uint32_t length) to read data from the external flash using CPU
     o Use readDataDMA(uint32_t address24, uint8_t* buffer, uint32_t length) to read data from the external flash using DMA
------------------------------------------------------------------------------*/

#include "MX25L6433.h"

#if BSP_DE_LITE_MX25L6433_H_ != 0100	/* BSP External Flash REVISION v.1.0 */
#error Wrong API version file (MX25L6433.h).
#endif

//References for the peripherals used to read the flash
static SPI_TypeDef* const FLASH_SPI = SPI2;
extern DMA_HandleTypeDef hdma_spi2_rx;
extern DMA_HandleTypeDef hdma_spi2_tx;


//Status flag. Non-zero when receiving data
static volatile uint8_t isReceivingData = 0;


/**
  * @brief  Read Data from External Flash
  * @param  uint32_t address24:The address from where the data to be read from the external flash
  * @param  uint8_t* buffer:The buffer pointer where the read data needs to be stored
  * @param  uint32_t length:The size of data (in bytes)
  */
void readData (uint32_t address24, uint8_t* buffer, uint32_t length)
{
    LL_SPI_Enable(FLASH_SPI);
    LL_GPIO_ResetOutputPin(FLASH_CSX_GPIO_Port, FLASH_CSX_Pin);

    while (!LL_SPI_IsActiveFlag_TXE(FLASH_SPI));  //Waits until the TXFIFO is Empty for the next Transmission.
    LL_SPI_TransmitData8(FLASH_SPI,MX25L_READ_DATA);
    while (!LL_SPI_IsActiveFlag_RXNE(FLASH_SPI));
    LL_SPI_ReceiveData8(FLASH_SPI); // Dummy read to clear RXNE

    while (!LL_SPI_IsActiveFlag_TXE(FLASH_SPI));
    LL_SPI_TransmitData8(FLASH_SPI, (address24 >> 16)&0xFF);
    while (!LL_SPI_IsActiveFlag_RXNE(FLASH_SPI));
    LL_SPI_ReceiveData8(FLASH_SPI); // Dummy read to clear RXNE

    LL_SPI_TransmitData8(FLASH_SPI, (address24 >> 8)&0xFF);
    while (!LL_SPI_IsActiveFlag_RXNE(FLASH_SPI));
    LL_SPI_ReceiveData8(FLASH_SPI); // Dummy read to clear RXNE

    LL_SPI_TransmitData8(FLASH_SPI, address24 &0xFF);
    while (!LL_SPI_IsActiveFlag_RXNE(FLASH_SPI));
    LL_SPI_ReceiveData8(FLASH_SPI); // Dummy read to clear RXNE

    while(length>0) {
    	while (!LL_SPI_IsActiveFlag_TXE(FLASH_SPI));
    	LL_SPI_TransmitData8(FLASH_SPI, 0x00); // Send dummy byte
    	 while (!LL_SPI_IsActiveFlag_RXNE(FLASH_SPI));  // Wait Until the RX FIFO has some data to read
         *buffer++ = LL_SPI_ReceiveData8(FLASH_SPI);
         length--;
    }


    /* Wait until the bus is ready before releasing Chip select */
    while(LL_SPI_IsActiveFlag_BSY(FLASH_SPI)) { }

	LL_GPIO_SetOutputPin(FLASH_CSX_GPIO_Port, FLASH_CSX_Pin);

}

/**
  * @brief  Read Data from External Flash using DMA
  * @param  uint32_t address24:The address from where the data to be read from the external flash
  * @param  uint8_t* buffer:The buffer pointer where the read data needs to be stored
  * @param  uint32_t length:The size of data (in bytes)
  */
void readDataDMA(uint32_t address24, uint8_t* buffer, uint32_t length)
{
	isReceivingData = 1;
	LL_SPI_Enable(FLASH_SPI);
	LL_GPIO_ResetOutputPin(FLASH_CSX_GPIO_Port, FLASH_CSX_Pin); //activate chip select
    LL_SPI_TransmitData8(FLASH_SPI,MX25L_READ_DATA );

    //clock out address
    LL_SPI_TransmitData8(FLASH_SPI, (address24 >> 16)&0xFF);

    while(!LL_SPI_IsActiveFlag_TXE(FLASH_SPI)) { }
    LL_SPI_TransmitData8(FLASH_SPI, (address24 >> 8)&0xFF);

    while(!LL_SPI_IsActiveFlag_TXE(FLASH_SPI)) { }
    LL_SPI_TransmitData8(FLASH_SPI, address24 &0xFF);

    /* Wait until the bus is ready before reading 4 dummy bytes */
    while(LL_SPI_IsActiveFlag_BSY(FLASH_SPI)) { }
    LL_SPI_ReceiveData8(FLASH_SPI); // Dummy read to clear RXNE
    LL_SPI_ReceiveData8(FLASH_SPI); // Dummy read to clear RXNE
    LL_SPI_ReceiveData8(FLASH_SPI); // Dummy read to clear RXNE
    LL_SPI_ReceiveData8(FLASH_SPI); // Dummy read to clear RXNE

	/* Reset the threshold bit */
	CLEAR_BIT(FLASH_SPI->CR2, SPI_CR2_LDMATX | SPI_CR2_LDMARX);

	/* Set RX Fifo threshold according the reception data length: 8bit */
	SET_BIT(FLASH_SPI->CR2, SPI_RXFIFO_THRESHOLD);

	/******** RX ****************/
	/* Disable the peripheral */
	__HAL_DMA_DISABLE(&hdma_spi2_rx);

	 /* Clear all flags */
	 __HAL_DMA_CLEAR_FLAG(&hdma_spi2_rx, (DMA_FLAG_GI1 << (hdma_spi2_rx.ChannelIndex & 0x1cU)));

	/* Configure DMA Channel data length */
	hdma_spi2_rx.Instance->CNDTR = length;

	/* Configure DMA Channel destination address */
	hdma_spi2_rx.Instance->CPAR = LL_SPI_DMA_GetRegAddr(FLASH_SPI);

	/* Configure DMA Channel source address */
	hdma_spi2_rx.Instance->CMAR = (uint32_t)buffer;

	__HAL_DMA_DISABLE_IT(&hdma_spi2_rx, DMA_IT_HT | DMA_IT_TE);
	__HAL_DMA_ENABLE_IT(&hdma_spi2_rx, (DMA_IT_TC));

	/* Enable the Peripheral */
	__HAL_DMA_ENABLE(&hdma_spi2_rx);

			/* Enable Rx DMA Request */
	LL_SPI_EnableDMAReq_RX(FLASH_SPI);

	/******** TX ****************/
	/* Disable the peripheral */
	__HAL_DMA_DISABLE(&hdma_spi2_tx);

	/* Clear all flags */
	__HAL_DMA_CLEAR_FLAG(&hdma_spi2_tx, (DMA_FLAG_GI1 << (hdma_spi2_tx.ChannelIndex & 0x1cU)));

	/* Configure DMA Channel data length */
	hdma_spi2_tx.Instance->CNDTR = length;

	/* Configure DMA Channel destination address */
	hdma_spi2_tx.Instance->CPAR = LL_SPI_DMA_GetRegAddr(FLASH_SPI);

	/* Configure DMA Channel source address */
	hdma_spi2_tx.Instance->CMAR = (uint32_t)buffer;

	/* Enable the Peripheral */
	__HAL_DMA_ENABLE(&hdma_spi2_tx);

	/* Enable Tx DMA Request */
	LL_SPI_EnableDMAReq_TX(FLASH_SPI);
}

/**
 * @brief Callback Function for External Flash
 */
void DataReader_DMACallback()
{
    /* Transfer Complete Interrupt management ***********************************/
    if ((0U != (DMA1->ISR & (DMA_FLAG_TC1 << (hdma_spi2_rx.ChannelIndex & 0x1cU)))) && (0U != (hdma_spi2_rx.Instance->CCR & DMA_IT_TC)))
    {
        /* Disable the transfer complete and error interrupt */
        __HAL_DMA_DISABLE_IT(&hdma_spi2_rx, DMA_IT_TE | DMA_IT_TC);
        /* Clear the transfer complete flag */
        __HAL_DMA_CLEAR_FLAG(&hdma_spi2_rx, (DMA_FLAG_TC1 << (hdma_spi2_rx.ChannelIndex & 0x1cU)));


        // Wait until the bus is not busy before changing configuration
        // SPI is busy in communication or Tx buffer is not empty
        while(LL_SPI_IsActiveFlag_BSY(FLASH_SPI)) { }

        LL_GPIO_SetOutputPin(GPIOB, GPIO_PIN_12);

        isReceivingData = 0;
    }
}

/**
  * @brief  Calls the readData function
  * @param  uint32_t address24:The address from where the data to be read from the external flash
  * @param  uint8_t* buffer:The buffer pointer where the read data needs to be stored
  * @param  uint32_t length:The size of data (in bytes)
  */
void DataReader_ReadData(uint32_t address24, uint8_t* buffer, uint32_t length)
{
    readData(address24-0x90000000, buffer, length);
}


/**
  * @brief  Calls the readDataDMA function
  * @param  uint32_t address24:The address from where the data to be read from the external flash
  * @param  uint8_t* buffer:The buffer pointer where the read data needs to be stored
  * @param  uint32_t length:The size of data (in bytes)
  */
void DataReader_StartDMAReadData(uint32_t address24, uint8_t* buffer, uint32_t length)
{
    readDataDMA(address24-0x90000000, buffer, length);
}


/**
 * @brief Wait until the data reception is complete
 */
void DataReader_WaitForReceiveDone(void)
{
    while (isReceivingData) { }
}
