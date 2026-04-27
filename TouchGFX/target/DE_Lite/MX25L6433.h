/*
 * MX25L6433.h
 *
 *  Created on: May 12, 2025
 *      Author: RenjithGopan
 */

#ifndef BSP_DE_LITE_MX25L6433_H_
#define BSP_DE_LITE_MX25L6433_H_ 0100 /* BSP External Flash API REVISION v.1.0 */

#include "main.h"
#include "bsp.h"


#ifdef __cplusplus
extern "C" {
#endif

#define MEMORY_FLASH_SIZE				0x4000000 /* 64Mbit =>8Mbyte */
#define MEMORY_BLOCK_SIZE				0x10000   /*  blocks of 64KBytes */
#define MEMORY_SECTOR_SIZE				0x1000    /* 4kbits =>500Bytes */
#define MEMORY_PAGE_SIZE				0x100     /* 256 bits => 32 bytes  */

#define MX25L_WRITE_ENABLE 0x06
#define MX25L_WRITE_DISABLE 0x04

#define MX25L_READ_SR1 0x05
#define MX25L_READ_SR2 0x35
#define MX25L_READ_SR3 0x15
#define MX25L_WRITE_SR1 0x01
#define MX25L_WRITE_SR2 0x31
#define MX25L_WRITE_SR3 0x11

#define MX25L_READ_DATA 0x03
#define MX25L_READ_DATA_4B 0x13
#define MX25L_FAST_READ 0x0B
#define MX25L_FAST_READ_4B 0x0C

#define MX25L_PAGE_PROGRAM 0x02
#define MX25L_PAGE_PROGRAM_4B 0x12

#define MX25L_SECTOR_ERASE 0x20
#define MX25L_SECTOR_ERASE_4B 0x21
#define MX25L_32KB_BLOCK_ERASE 0x52
#define MX25L_64KB_BLOCK_ERASE 0xD8
#define MX25L_64KB_BLOCK_ERASE_4B 0xDC
#define MX25L_CHIP_ERASE 0xC7

#define MX25L_ENABLE_RST 0x66
#define MX25L_RESET 0x99


void MX25L_Read (uint32_t startPage, uint8_t offset, uint32_t size, uint8_t *rData);
void MX25L_FastRead (uint32_t startPage, uint8_t offset, uint32_t size, uint8_t *rData);
void MX25L_FastRead_DMA (uint32_t startPage, uint8_t offset, uint32_t size, uint8_t *rData);

uint32_t DataReader_IsReceivingData(void);
void     DataReader_WaitForReceiveDone(void);
void     DataReader_ReadData(uint32_t address24, uint8_t* buffer, uint32_t length);
void     DataReader_StartDMAReadData(uint32_t address24, uint8_t* buffer, uint32_t length);
void     DataReader_DMACallback(void);


#ifdef __cplusplus
}
#endif

#endif /* BSP_DE_LITE_MX25L6433_H_ */
