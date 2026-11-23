/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          	: STM32TouchController.cpp
  ******************************************************************************
  * This file was created by TouchGFX Generator 4.26.0. This file is only
  * generated once! Delete this file from your project and re-generate code
  * using STM32CubeMX or change this file manually to update it.
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

/******************************************************************************
 * Change History Table
 * -----------------------------------------------------------------------------
 * Version | Date       | Author       | Description
 * -----------------------------------------------------------------------------
 * 1.0     | 2025-10-07 | H.Hunain  | Added driver for st1x33i
 * 1.1     | 2025-10-07 | H.Hunain  | Added driver for DATAIMAGE_MICROCHIP_TOUCH_IC
 * 1.2     | 2026-04-08 | R.Gopan   | Added driver for st1x33i_240x240 and enhanced portability
 * 1.3	   | 2026-25-08 | H.Hunain  | Backwards compatibility fixed for product range
 * 1.4     | 2026-11-02 | H.Hunain  | Added st1x33i_800x480 for the 5" DEU5_5R (800x480)
 * 1.5     | 2026-11-04 | R.Gopan   | Removed stm32u5xx_hal.h from the header
 * 1.6     | 2026-11-20 | R.Gopan   | Added DragonEyeH5 and DragonEyeG0 support
 * -----------------------------------------------------------------------------
 * Product selection (bsp.h):
 *   DragonEye2.0 : #define DEU5_2_1R   + #define st1x33i_240x240/480x480/360x360/800x480  (STM32U5, hi2c5)
 *   1.3" G0      : #define DragonEyeG0 + #define st1x33i_240x240/480x480/360x360/800x480  (STM32G0, hi2c2)
 *   1.8" H5      : #define DragonEyeH5 + #define st1x33i_240x240/480x480/360x360/800x480  (STM32H5, bit-banged I2C on PB3/PB4)
 * -----------------------------------------------------------------------------
 * Notes:
 * - Always increment version when making manual changes.
 * - Describe clearly what was changed in each version.
 ******************************************************************************/

/* USER CODE END Header */

/* USER CODE BEGIN STM32TouchController */
#include <STM32TouchController.hpp>
#include "main.h"
#include "bsp.h"
#include "math.h"

#if __TOUCH_CONTROLLER_HPP != 0106	/* BSP Touch REVISION v.1.7 */
#error Wrong API version file (STM32TouchController.hpp).
#endif

#if defined(DEU5_2_1R) && (defined(DragonEyeG0) || defined(DragonEyeH5))
#error "bsp.h: define only one of DEU5_2_1R (DragonEye2.0) / DragonEyeG0 (1.3in G0) / DragonEyeH5 (1.8in H5)"
#endif

#ifdef DragonEyeH5
/* ===================== 1.8" H5: touch bus =====================
 * The ST1633 is on PB3 (SCL) / PB4 (SDA). On the H5 these pins belong to I3C2 (CubeMX) and the project has no
 * HAL I2C module.
 * H5 reads the chip with a bit-banged (software) I2C master on the same pins, which waits for clock
 * stretching like a hardware I2C.
 * Used by the DragonEyeH5 versions of TS_IO_Write / TS_IO_Read / TS_IO_ReadMultiple further down. */
/* ---------------------------------------------------------------- */
#define TS_SCL_PORT        GPIOB
#define TS_SCL_PIN         GPIO_PIN_3
#define TS_SDA_PORT        GPIOB
#define TS_SDA_PIN         GPIO_PIN_4
#define TS_I2C_HALF_US     5U        /* half clock period: 5 us = 100 kHz        */
#define TS_I2C_STRETCH_US  2000U     /* longest time the chip may hold SCL low   */

#ifdef HAL_I3C_MODULE_ENABLED
extern I3C_HandleTypeDef hi3c2;      /* CubeMX set PB3 / PB4 up for I3C2; released in ts_softInit() */
#endif

static bool     ts_softReady   = false;
static uint32_t ts_cyclesPerUs = 1U;

static void ts_delayUs(uint32_t us)
{
	const uint32_t start = DWT->CYCCNT;
	const uint32_t n = us * ts_cyclesPerUs;
	while ((DWT->CYCCNT - start) < n) { }
}

static inline void ts_sclLow(void)  { TS_SCL_PORT->BRR  = TS_SCL_PIN; }
static inline void ts_sdaLow(void)  { TS_SDA_PORT->BRR  = TS_SDA_PIN; }
static inline void ts_sdaRel(void)  { TS_SDA_PORT->BSRR = TS_SDA_PIN; }      /* open-drain: release = high */
static inline bool ts_sda(void)     { return (TS_SDA_PORT->IDR & TS_SDA_PIN) != 0U; }

/* release SCL, wait while the chip stretches it, then hold the high time */
static bool ts_sclHigh(void)
{
	TS_SCL_PORT->BSRR = TS_SCL_PIN;
	const uint32_t start = DWT->CYCCNT;
	const uint32_t n = TS_I2C_STRETCH_US * ts_cyclesPerUs;
	while ((TS_SCL_PORT->IDR & TS_SCL_PIN) == 0U) {
		if ((DWT->CYCCNT - start) > n) {
			return false;
		}
	}
	ts_delayUs(TS_I2C_HALF_US);
	return true;
}

static bool ts_start(void)          /* also used as repeated start (SCL is low then) */
{
	ts_sdaRel();
	ts_delayUs(TS_I2C_HALF_US);
	if (!ts_sclHigh()) {
		return false;
	}
	ts_sdaLow();
	ts_delayUs(TS_I2C_HALF_US);
	ts_sclLow();
	ts_delayUs(TS_I2C_HALF_US);
	return true;
}

static void ts_stop(void)
{
	ts_sdaLow();
	ts_delayUs(TS_I2C_HALF_US);
	(void)ts_sclHigh();
	ts_sdaRel();
	ts_delayUs(TS_I2C_HALF_US);
}

static bool ts_writeByte(uint8_t b)  /* true = ACK */
{
	for (uint8_t i = 0; i < 8U; i++) {
		if (b & 0x80U) { ts_sdaRel(); } else { ts_sdaLow(); }
		b <<= 1;
		ts_delayUs(TS_I2C_HALF_US);
		if (!ts_sclHigh()) {
			return false;
		}
		ts_sclLow();
	}
	ts_sdaRel();
	ts_delayUs(TS_I2C_HALF_US);
	if (!ts_sclHigh()) {
		return false;
	}
	const bool ack = !ts_sda();
	ts_sclLow();
	return ack;
}

static bool ts_readByte(uint8_t *b, bool ack)
{
	uint8_t v = 0;
	ts_sdaRel();
	for (uint8_t i = 0; i < 8U; i++) {
		ts_delayUs(TS_I2C_HALF_US);
		if (!ts_sclHigh()) {
			return false;
		}
		v = (uint8_t)((v << 1) | (ts_sda() ? 1U : 0U));
		ts_sclLow();
	}
	if (ack) { ts_sdaLow(); } else { ts_sdaRel(); }
	ts_delayUs(TS_I2C_HALF_US);
	if (!ts_sclHigh()) {
		return false;
	}
	ts_sclLow();
	ts_sdaRel();
	*b = v;
	return true;
}

/* free the bus if the chip was left holding SDA (e.g. after a reset in the middle of a read) */
static void ts_busClear(void)
{
	for (uint8_t i = 0; i < 9U && !ts_sda(); i++) {
		ts_sclLow();
		ts_delayUs(TS_I2C_HALF_US);
		(void)ts_sclHigh();
	}
	ts_stop();
}

/* first use: release PB3 / PB4 from I3C2 and make them open-drain GPIOs */
static void ts_softInit(void)
{
	GPIO_InitTypeDef g = {0};

#ifdef HAL_I3C_MODULE_ENABLED
	(void)HAL_I3C_DeInit(&hi3c2);
#endif

	DCB->DEMCR |= DCB_DEMCR_TRCENA_Msk;
	DWT->CTRL  |= DWT_CTRL_CYCCNTENA_Msk;
	ts_cyclesPerUs = SystemCoreClock / 1000000U;
	if (ts_cyclesPerUs == 0U) {
		ts_cyclesPerUs = 1U;
	}

	TS_SCL_PORT->BSRR = TS_SCL_PIN;
	TS_SDA_PORT->BSRR = TS_SDA_PIN;
	g.Mode  = GPIO_MODE_OUTPUT_OD;
	g.Pull  = GPIO_PULLUP;
	g.Speed = GPIO_SPEED_FREQ_LOW;
	g.Pin   = TS_SCL_PIN;
	HAL_GPIO_Init(TS_SCL_PORT, &g);
	g.Pin   = TS_SDA_PIN;
	HAL_GPIO_Init(TS_SDA_PORT, &g);

	ts_busClear();
	ts_softReady = true;
}

/* Addr = 8-bit address (I2C_Address = ST1633I_ADD_REG<<1). Returns HAL_OK / HAL_ERROR, never hangs. */
static HAL_StatusTypeDef TS_SOFT_MemWrite(uint16_t Addr, uint16_t Reg, uint8_t *pData, uint16_t Size)
{
	if (!ts_softReady) {
		ts_softInit();
	}
	bool ok = ts_start() && ts_writeByte((uint8_t)(Addr & 0xFEU)) && ts_writeByte((uint8_t)Reg);
	for (uint16_t i = 0; ok && i < Size; i++) {
		ok = ts_writeByte(pData[i]);
	}
	ts_stop();
	if (!ok) {
		ts_busClear();
	}
	return ok ? HAL_OK : HAL_ERROR;
}

static HAL_StatusTypeDef TS_SOFT_MemRead(uint16_t Addr, uint16_t Reg, uint8_t *pData, uint16_t Size)
{
	if (!ts_softReady) {
		ts_softInit();
	}
	bool ok = ts_start() && ts_writeByte((uint8_t)(Addr & 0xFEU)) && ts_writeByte((uint8_t)Reg)
	          && ts_start() && ts_writeByte((uint8_t)(Addr | 0x01U));
	for (uint16_t i = 0; ok && i < Size; i++) {
		ok = ts_readByte(&pData[i], (i + 1U) < Size);
	}
	ts_stop();
	if (!ok) {
		ts_busClear();
	}
	return ok ? HAL_OK : HAL_ERROR;
}

#endif /* DragonEyeH5 */

/* ---- I2C bus of the touch chip, per product ---- */
#if defined(DEU5_2_1R) || defined(DATAIMAGE_MICROCHIP_TOUCH_IC)
extern I2C_HandleTypeDef hi2c5;
#endif
#ifdef DEU5_2_1R
#define TS_I2C_HANDLE   hi2c5             /* DragonEye2.0 */
#elif defined(DragonEyeH5)
/* 1.8" H5: no I2C handle, the touch chip is read with the bit-banged I2C (DragonEyeH5 touch bus above) */
#else
extern I2C_HandleTypeDef hi2c2;
#define TS_I2C_HANDLE   hi2c2             /* 1.3" G0 and other G0-style products */
#endif

static bool TS_initialized = false;
volatile uint8_t Touch_Event_Flag = 0;

#if defined(st1x33i_240x240) || defined(st1x33i_360x360) || defined(st1x33i_480x480) || defined(st1x33i_800x480)/* any ST1633i: st1x33i, st1x33i_240x240/480x480/800x480 */
static TS_StateTypeDef  TS_State = {0};
touch_data_t ST_1633ITouchData;
uint8_t  I2C_Address = ST1633I_ADD_REG<<1;
uint8_t cord[3]={0};


#endif

#if defined(DragonEyeG0) || defined(DragonEyeH5)
extern uint8_t ui8EncoderPushed;
static bool    fingerDown = false;
static bool    touchMoved = false;
#endif

#ifdef DATAIMAGE_MICROCHIP_TOUCH_IC
static bool Touch_State = false;
uint8_t check_pic16f1885;
static bool touch_active = false;
static uint32_t touch_start_time = 0;
#endif
extern "C" void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

using namespace touchgfx;

#ifdef DEU5_2_1R
extern "C" volatile uint8_t haptic_feedback;
uint32_t old = 0;
uint8_t touch_happened = 0;
#endif




void STM32TouchController::init()
{
	if(!TS_initialized)
	{
#ifdef st1x33i_240x240
		BSP_TS_Init(240,240);
#endif
#ifdef st1x33i_360x360
		BSP_TS_Init(360,360);
#endif
#ifdef st1x33i_480x480
		BSP_TS_Init(480,480);
#endif
#ifdef st1x33i_800x480
		BSP_TS_Init(800,480);
#endif
		TS_initialized = true;
	}
}

bool STM32TouchController::sampleTouch(int32_t& x, int32_t& y)
{
#if defined(DEU5_2_1R)
	/* ===================== DragonEye2.0 (unchanged) ===================== */

	static int32_t lastX = 0;
	static int32_t lastY = 0;

	if (Touch_Event_Flag == 1){
			Touch_Event_Flag = 0;
			TS_IO_ReadMultiple(I2C_Address, 0x12, cord, sizeof(cord));
			ST1633I_TouchDtect(&ST_1633ITouchData, cord);
	}

	if (BSP_TS_GetState(&TS_State) == TOUCH_OK){
		if(TS_State.touchDetected){
			if(touch_happened==0){
				touch_happened = 1;
				haptic_feedback = 2;
			}

			int32_t newX = TS_State.touchX[0];
			int32_t newY = TS_State.touchY[0];

			// ---- Jitter filter (dead zone of 15 pixels) ----
			if (std::abs(newX - lastX) < 15 && std::abs(newY - lastY) < 15)
			{
				newX = lastX;
				newY = lastY;
			}

			lastX = newX;
			lastY = newY;

			x = lastX;
			y = lastY;
			return true;
		}
	}
	if(touch_happened==1){
		touch_happened = 0;
	}
#elif defined(DragonEyeH5)
	/* ===================== 1.8" H5: tap / drag (same as the 1.3" G0 for now) ===================== */
	static int32_t lastX = 0;
	static int32_t lastY = 0;

	if (Touch_Event_Flag == 1){
		Touch_Event_Flag = 0;
		TS_IO_ReadMultiple(I2C_Address, 0x12, cord, sizeof(cord));
		ST1633I_TouchDtect(&ST_1633ITouchData, cord);
	}

	if (BSP_TS_GetState(&TS_State) == TOUCH_OK){

		if(TS_State.touchDetected){
			int32_t newX = TS_State.touchX[0];
			int32_t newY = TS_State.touchY[0];
#ifdef st1x33i_800x480
			/* 5" panel: if touches come out rotated or mirrored, define TS_SWAP_XY,
			 * TS_MIRROR_X and/or TS_MIRROR_Y in bsp.h */
#ifdef TS_SWAP_XY
			{ int32_t t = newX; newX = newY; newY = t; }
#endif
#ifdef TS_MIRROR_X
			newX = (ST_1633I_MAX_WIDTH - 1) - newX;
#endif
#ifdef TS_MIRROR_Y
			newY = (ST_1633I_MAX_HEIGHT - 1) - newY;
#endif
			if (newX < 0) newX = 0;
			if (newY < 0) newY = 0;
			if (newX > ST_1633I_MAX_WIDTH - 1)  newX = ST_1633I_MAX_WIDTH - 1;
			if (newY > ST_1633I_MAX_HEIGHT - 1) newY = ST_1633I_MAX_HEIGHT - 1;
#endif

			if (!fingerDown) {
				ui8EncoderPushed = ENC_PB_RELEASED;
				fingerDown = true;
				touchMoved = false;
				x= lastX = newX;
				y= lastY = newY;
			}
			// ---- Jitter filter (dead zone of 15 pixels) ----
			else if (abs(newX - lastX) >= 15 || abs(newY - lastY) >= 15)
			{
				x= lastX =newX;
				y= lastY =newY;
				touchMoved = true; //Drag
			}
			else{
				x = lastX;
				y = lastY;
			}
			return true;
		}
	}
	/* chip says lifted */
		if (fingerDown) {
			fingerDown = false;
			if (!touchMoved) {
				ui8EncoderPushed = ENC_PB_PUSHED;
			}
		}

#elif defined(DragonEyeG0)
	static int32_t lastX = 0;
	static int32_t lastY = 0;

	if (Touch_Event_Flag == 1){
		Touch_Event_Flag = 0;
		TS_IO_ReadMultiple(I2C_Address, 0x12, cord, sizeof(cord));
		ST1633I_TouchDtect(&ST_1633ITouchData, cord);
	}

	if (BSP_TS_GetState(&TS_State) == TOUCH_OK){

		if(TS_State.touchDetected){
			int32_t newX = TS_State.touchX[0];
			int32_t newY = TS_State.touchY[0];
#ifdef st1x33i_800x480
			/* 5" panel: if touches come out rotated or mirrored, define TS_SWAP_XY,
			 * TS_MIRROR_X and/or TS_MIRROR_Y in bsp.h */
#ifdef TS_SWAP_XY
			{ int32_t t = newX; newX = newY; newY = t; }
#endif
#ifdef TS_MIRROR_X
			newX = (ST_1633I_MAX_WIDTH - 1) - newX;
#endif
#ifdef TS_MIRROR_Y
			newY = (ST_1633I_MAX_HEIGHT - 1) - newY;
#endif
			if (newX < 0) newX = 0;
			if (newY < 0) newY = 0;
			if (newX > ST_1633I_MAX_WIDTH - 1)  newX = ST_1633I_MAX_WIDTH - 1;
			if (newY > ST_1633I_MAX_HEIGHT - 1) newY = ST_1633I_MAX_HEIGHT - 1;
#endif

			if (!fingerDown) {
				ui8EncoderPushed = ENC_PB_RELEASED;
				fingerDown = true;
				touchMoved = false;
				x= lastX = newX;
				y= lastY = newY;
			}
			// ---- Jitter filter (dead zone of 15 pixels) ----
			else if (abs(newX - lastX) >= 15 || abs(newY - lastY) >= 15)
			{
				x= lastX =newX;
				y= lastY =newY;
				touchMoved = true; //Drag
			}
			else{
				x = lastX;
				y = lastY;
			}
			return true;
		}
	}
	/* chip says lifted */
		if (fingerDown) {
			fingerDown = false;
			if (!touchMoved) {
				ui8EncoderPushed = ENC_PB_PUSHED;
			}
		}
#endif
#ifdef DATAIMAGE_MICROCHIP_TOUCH_IC

	if (Touch_State == true)
	{
		HAL_I2C_Mem_Read(&hi2c5, 0x28 << 1, 0x01, I2C_MEMADD_SIZE_8BIT, &check_pic16f1885, 1, HAL_MAX_DELAY);
		if(check_pic16f1885 == 0){
			Touch_State = false;
			touch_active = false;
		} else {
			if (!touch_active)
			        {
			            // First detection, record start time
			            touch_active = true;
			            touch_start_time = HAL_GetTick();
			        }
			else {
				 // Already active, check duration
				  if (HAL_GetTick() - touch_start_time >= 200)
				  {
					  x=5;
					  y=5;
					  Touch_State = false;
					  touch_active = false;
					  return true;  // Valid touch detected
				  }
			}
		}
	}
	return false;
#endif
    return false;
}



#if defined(st1x33i_240x240) || defined(st1x33i_360x360) || defined(st1x33i_480x480) || defined(st1x33i_800x480)

#ifdef DragonEyeH5
void TS_IO_Write(uint8_t Addr, uint8_t Reg, uint8_t Value) {
	TS_SOFT_MemWrite(Addr, Reg, &Value, 1);                    // 1.8" H5: bit-banged I2C
}

uint8_t  TS_IO_Read(uint8_t Addr, uint8_t Reg) {
	uint8_t Value = 0;
	TS_SOFT_MemRead(Addr, Reg, &Value, 1);
	return Value;
}

uint16_t TS_IO_ReadMultiple(uint8_t Addr, uint8_t Reg, uint8_t *Buffer, uint16_t Length) {
	return TS_SOFT_MemRead(Addr, Reg, Buffer, Length);
}
#else
void TS_IO_Write(uint8_t Addr, uint8_t Reg, uint8_t Value) {
	HAL_I2C_Mem_Write(&TS_I2C_HANDLE, Addr, (uint16_t) Reg, I2C_MEMADD_SIZE_8BIT, &Value, 1, 10000);
}

uint8_t  TS_IO_Read(uint8_t Addr, uint8_t Reg) {
	uint8_t Value = 0;
	HAL_I2C_Mem_Read(&TS_I2C_HANDLE, Addr, (uint16_t) Reg, I2C_MEMADD_SIZE_8BIT, &Value, 1, 10000);
	return Value;
}

uint16_t TS_IO_ReadMultiple(uint8_t Addr, uint8_t Reg, uint8_t *Buffer, uint16_t Length) {
	return HAL_I2C_Mem_Read(&TS_I2C_HANDLE, Addr, (uint16_t) Reg, I2C_MEMADD_SIZE_8BIT, Buffer, Length, 10000);
}
#endif

/* All products: valid taken from every report (1 = finger on the glass, 0 = lifted) */
void ST1633I_TouchDtect (touch_data_t * tdata , uint8_t *cordi)
{

	tdata->xyz_data[0].xy_h = cordi[0];
	tdata->xyz_data[0].ValidFlage = tdata->xyz_data[0].xyh_bit.valid;
		if(tdata->xyz_data[0].xyh_bit.valid)
		{

		tdata->xyz_data[0].x = cordi[1] | (cordi[0] & 0x70)<<4;
		tdata->xyz_data[0].y = cordi[2] | (cordi[0] & 0x07)<<8;
		}
		else
		{
			if(tdata->xyz_data[0].ValidFlage == 0)
			{
				tdata->xyz_data[0].x = 0;
				tdata->xyz_data[0].y = 0;
			}
		}
}


/**
 * @brief Initializes the TS_1633I touch sensor
 *
 * This function initializes the TS_1633I touch sensor by calling the power initialization function, introducing a delay, and performing further sensor-specific initialization.
 *
 * @param ts_SizeX The size of the touch sensor in the X-direction
 * @param ts_SizeY The size of the touch sensor in the Y-direction
 * @return The status of the touch sensor initialization
 */
uint8_t BSP_TS_Init(uint16_t ts_SizeX, uint16_t ts_SizeY)
{
	uint8_t ts_status = TOUCH_OK;
#if	defined(st1x33i_240x240) || defined(st1x33i_360x360)
	HAL_GPIO_WritePin(CTP_RST_GPIO_Port, CTP_RST_Pin, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(CTP_RST_GPIO_Port, CTP_RST_Pin, GPIO_PIN_SET);
	HAL_Delay(60);
#endif
	TS_IO_Write(I2C_Address,ST1633I_TD_CONTROL_REG_RW,ST1633I_TD_MULTOUCH_DIS);
	return (ts_status);
}

/**
  * @brief  Returns status and positions of the touch screen.
  * @param  TS_State: Pointer to touch screen current state structure
  * @retval TOUCH_OK if all initializations are OK. Other value if error.
  */
/* All products: keeps ValidFlage until the chip reports a lift */
uint8_t BSP_TS_GetState(TS_StateTypeDef *TS_State)
{
	uint8_t ts_status = TOUCH_OK;

	if(ST_1633ITouchData.xyz_data[0].ValidFlage)
	{
		TS_State->touchDetected = ST_1633ITouchData.xyz_data[0].ValidFlage;
		TS_State->touchX[0] = ST_1633ITouchData.xyz_data[0].x;
		TS_State->touchY[0] = ST_1633ITouchData.xyz_data[0].y;
	}
	else
	{
		TS_State->touchDetected = 0;
	}

	return ts_status;
}

uint8_t BSP_TS_ResetTouchData(TS_StateTypeDef *TS_State)
{

	ST_1633ITouchData.xyz_data[0].x = 0;
	ST_1633ITouchData.xyz_data[0].y = 0;
	ST_1633ITouchData.xyz_data[0].ValidFlage = 0 ;
	for(uint8_t coc=0;coc<sizeof(cord);coc++)
	{
		cord[coc]=0;
	}
	return TOUCH_OK;
}
#endif




#ifdef ft6x06

static TS_DrvTypeDef *ts_driver;

/**
 * @brief Initializes the touch sensor (other than TS_1633I)
 *
 * This function initializes a touch sensor other than TS_1633I by performing necessary initialization steps specific to the alternative touch sensor.
 *
 * @param ts_SizeX The size of the touch sensor in the X-direction
 * @param ts_SizeY The size of the touch sensor in the Y-direction
 * @return The status of the touch sensor initialization
 */
uint8_t BSP_TS_Init(uint16_t ts_SizeX, uint16_t ts_SizeY)
{
  uint8_t ts_status = TOUCH_OK;
  uint8_t ts_id1, ts_id2 = 0;
  /* Note : I2C_Address is un-initialized here, but is not used at all in init function */
  /* but the prototype of Init() is like that in template and should be respected       */

  /* Initialize the communication channel to sensor (I2C) if necessary */
  /* that is initialization is done only once after a power up         */
  ft6x06_ts_drv.Init(I2C_Address);

  ts_id1 = ft6x06_ts_drv.ReadID(TS_I2C_ADDRESS);
  if(ts_id1 != FT6206_ID_VALUE)
  {
    ts_id2 = ft6x06_ts_drv.ReadID(TS_I2C_ADDRESS_A02);
    I2C_Address    = TS_I2C_ADDRESS_A02;
  }
  else
  {
    I2C_Address    = TS_I2C_ADDRESS;
  }

  /* Scan FT6xx6 TouchScreen IC controller ID register by I2C Read       */
  /* Verify this is a FT6206 or FT6336G, otherwise this is an error case */
  if((ts_id1 == FT6206_ID_VALUE) || (ts_id2 == FT6206_ID_VALUE) || (ts_id2 == FT6X36_ID2_VALUE))
  {
    /* Found FT6206 : Initialize the TS driver structure */
    ts_driver = &ft6x06_ts_drv;

    /* Get LCD chosen orientation */
    if(ts_SizeX < ts_SizeY)
    {
      ts_orientation = TS_SWAP_NONE;
    }
    else
    {
      ts_orientation = TS_SWAP_XY | TS_SWAP_Y;
    }

    if(ts_status == TOUCH_OK)
    {
      /* Software reset the TouchScreen */
      ts_driver->Reset(I2C_Address);

      /* Calibrate, Configure and Start the TouchScreen driver */
      ts_driver->Start(I2C_Address);

    } /* of if(ts_status == TOUCH_OK) */
  }
  else
  {
    ts_status = TS_DEVICE_NOT_FOUND;
  }

  return (ts_status);
}

/**
  * @brief  Update gesture Id following a touch detected.
  * @param  TS_State: Pointer to touch screen current state structure
  * @retval TOUCH_OK if all initializations are OK. Other value if error.
  */
uint8_t BSP_TS_Get_GestureId(TS_StateTypeDef *TS_State)
{
  uint32_t gestureId = 0;
  uint8_t  ts_status = TOUCH_OK;

  /* Get gesture Id */
  ft6x06_TS_GetGestureID(I2C_Address, &gestureId);

  /* Remap gesture Id to a TS_GestureIdTypeDef value */
  switch(gestureId)
  {
    case FT6206_GEST_ID_NO_GESTURE :
      TS_State->gestureId = GEST_ID_NO_GESTURE;
      break;
    case FT6206_GEST_ID_MOVE_UP :
      TS_State->gestureId = GEST_ID_MOVE_UP;
      break;
    case FT6206_GEST_ID_MOVE_RIGHT :
      TS_State->gestureId = GEST_ID_MOVE_RIGHT;
      break;
    case FT6206_GEST_ID_MOVE_DOWN :
      TS_State->gestureId = GEST_ID_MOVE_DOWN;
      break;
    case FT6206_GEST_ID_MOVE_LEFT :
      TS_State->gestureId = GEST_ID_MOVE_LEFT;
      break;
    case FT6206_GEST_ID_ZOOM_IN :
      TS_State->gestureId = GEST_ID_ZOOM_IN;
      break;
    case FT6206_GEST_ID_ZOOM_OUT :
      TS_State->gestureId = GEST_ID_ZOOM_OUT;
      break;
    default :
      ts_status = TOUCH_ERROR;
      break;
  } /* of switch(gestureId) */

  return(ts_status);
}


/** @defgroup STM32469I-Discovery_TS_Private_Functions STM32469I Discovery TS Private Functions
  * @{
  */


/**
  * @brief  Function used to reset all touch data before a new acquisition
  *         of touch information.
  * @param  TS_State: Pointer to touch screen current state structure
  * @retval TOUCH_OK if OK, TE_ERROR if problem found.
  */
uint8_t BSP_TS_ResetTouchData(TS_StateTypeDef *TS_State)
{
  uint8_t ts_status = TOUCH_ERROR;
  uint32_t index;

  if (TS_State != (TS_StateTypeDef *)NULL)
  {
    TS_State->gestureId = GEST_ID_NO_GESTURE;
    TS_State->touchDetected = 0;

    for(index = 0; index < TS_MAX_NB_TOUCH; index++)
    {
      TS_State->touchX[index]       = 0;
      TS_State->touchY[index]       = 0;
      TS_State->touchArea[index]    = 0;
      TS_State->touchEventId[index] = TOUCH_EVENT_NO_EVT;
      TS_State->touchWeight[index]  = 0;
    }

    ts_status = TOUCH_OK;

  } /* of if (TS_State != (TS_StateTypeDef *)NULL) */

  return (ts_status);

}

uint8_t BSP_TS_GetState(TS_StateTypeDef *TS_State)
{
	  static uint32_t _x[TS_MAX_NB_TOUCH] = {0, 0};
	  static uint32_t _y[TS_MAX_NB_TOUCH] = {0, 0};

	  uint16_t tmp;
	  uint16_t Raw_x[TS_MAX_NB_TOUCH];
	  uint16_t Raw_y[TS_MAX_NB_TOUCH];
	  uint16_t xDiff;
	  uint16_t yDiff;
	  uint32_t index;
	#if (TS_MULTI_TOUCH_SUPPORTED == 1)
	  uint32_t weight = 0;
	  uint32_t area = 0;
	  uint32_t event = 0;
	#endif /* TS_MULTI_TOUCH_SUPPORTED == 1 */

	  /* Check and update the number of touches active detected */
	  TS_State->touchDetected = ts_driver->DetectTouch(I2C_Address);
	  if(TS_State->touchDetected)
	  {
	    for(index=0; index < TS_State->touchDetected; index++)
	    {
	      /* Get each touch coordinates */
	      ts_driver->GetXY(I2C_Address, &(Raw_x[index]), &(Raw_y[index]));

	      if(ts_orientation & TS_SWAP_XY)
	      {
	        tmp = Raw_x[index];
	        Raw_x[index] = Raw_y[index];
	        Raw_y[index] = tmp;
	      }

	      if(ts_orientation & TS_SWAP_X)
	      {
	        Raw_x[index] = FT_6206_MAX_WIDTH - 1 - Raw_x[index];
	      }

	      if(ts_orientation & TS_SWAP_Y)
	      {
	        Raw_y[index] = FT_6206_MAX_HEIGHT - 1 - Raw_y[index];
	      }

	      xDiff = Raw_x[index] > _x[index]? (Raw_x[index] - _x[index]): (_x[index] - Raw_x[index]);
	      yDiff = Raw_y[index] > _y[index]? (Raw_y[index] - _y[index]): (_y[index] - Raw_y[index]);

	      if ((xDiff + yDiff) > 5)
	      {
	        _x[index] = Raw_x[index];
	        _y[index] = Raw_y[index];
	      }


	      TS_State->touchX[index] = _x[index];
	      TS_State->touchY[index] = _y[index];

	#if (TS_MULTI_TOUCH_SUPPORTED == 1)

	      /* Get touch info related to the current touch */
	      ft6x06_TS_GetTouchInfo(I2C_Address, index, &weight, &area, &event);

	      /* Update TS_State structure */
	      TS_State->touchWeight[index] = weight;
	      TS_State->touchArea[index]   = area;

	      /* Remap touch event */
	      switch(event)
	      {
	        case FT6206_TOUCH_EVT_FLAG_PRESS_DOWN  :
	          TS_State->touchEventId[index] = TOUCH_EVENT_PRESS_DOWN;
	          break;
	        case FT6206_TOUCH_EVT_FLAG_LIFT_UP :
	          TS_State->touchEventId[index] = TOUCH_EVENT_LIFT_UP;
	          break;
	        case FT6206_TOUCH_EVT_FLAG_CONTACT :
	          TS_State->touchEventId[index] = TOUCH_EVENT_CONTACT;
	          break;
	        case FT6206_TOUCH_EVT_FLAG_NO_EVENT :
	          TS_State->touchEventId[index] = TOUCH_EVENT_NO_EVT;
	          break;
	        default :
	          ts_status = TOUCH_ERROR;
	          break;
	      } /* of switch(event) */

	#endif /* TS_MULTI_TOUCH_SUPPORTED == 1 */

	    } /* of for(index=0; index < TS_State->touchDetected; index++) */

	#if (TS_MULTI_TOUCH_SUPPORTED == 1)
	    /* Get gesture Id */
	    ts_status = BSP_TS_Get_GestureId(TS_State);
	#endif /* TS_MULTI_TOUCH_SUPPORTED == 1 */

	  } /* end of if(TS_State->touchDetected != 0) */

	  return (ts_status);
}
#endif

#if FT5426 || ILI2130 || ILI2511

void STM32TouchController::getDriver ( void )
{
	uint8_t Buf[ 10 ];

	TP.driver = unknown;

	if( HAL_I2C_Mem_Read( &hi2c1, 0x70, 0xA3, I2C_MEMADD_SIZE_8BIT, &Buf[0], 1, 10 ) == HAL_OK )
	{
		if( Buf[0] == 0x0A ) // FT5x16
		{
		}
		else if( Buf[0] == 0x54 )
			TP.driver = FT5426;
		else if( Buf[0] == 0x55 ) // FT5x06
		{
		}
		else {}
	}
	else if( HAL_I2C_Mem_Read( &hi2c1, 0x82, 0x40, I2C_MEMADD_SIZE_8BIT, &Buf[0], 8, 10 ) == HAL_OK )
	{
		if( Buf[0] == 0x06 )
			TP.driver = ILI2511;
		if( Buf[0] == 0x07 )
			TP.driver = ILI2130;
	}
	else {
	}
}

void STM32TouchController::getRange ( void )
{
	uint8_t Buf[ 15 ];

	switch( TP.driver )
	{
		case FT5426:
			TP.range.x = 1792;
			TP.range.y = 1024;
			break;
		case ILI2130:
			if( HAL_I2C_Mem_Read( &hi2c1, 0x82, 0x20, I2C_MEMADD_SIZE_8BIT, &Buf[0], 10, 10 ) != HAL_OK )
				return;

			if( (Buf[8] == 0) || (Buf[8] > 32) )
				return;

			TP.range.x = *(uint16_t*)&Buf[0];
			TP.range.y = *(uint16_t*)&Buf[2];
			break;
		case ILI2511:
			if( HAL_I2C_Mem_Read( &hi2c1, 0x82, 0x20, I2C_MEMADD_SIZE_8BIT, &Buf[0], 10, 10 ) != HAL_OK )
				return;

			if( (Buf[6] == 0) || (Buf[6] == 255) )
				return;

			TP.range.x = *(uint16_t*)&Buf[0];
			TP.range.y = *(uint16_t*)&Buf[2];
			break;
		default:
			return;
	}
}

void STM32TouchController::getVersion ( void )
{
	uint8_t Buf[ 10 ];
	switch( TP.driver )
	{
		case FT5426:
			if( HAL_I2C_Mem_Read( &hi2c1, 0x70, 0xA1, I2C_MEMADD_SIZE_8BIT, &Buf[0], 2, 10 ) == HAL_OK )
			{
				TP.version = ( Buf[0] << 8 ) + Buf[1];
			}
			break;
		case ILI2130:
			if( HAL_I2C_Mem_Read( &hi2c1, 0x82, 0x42, I2C_MEMADD_SIZE_8BIT, &Buf[0], 3, 10 ) == HAL_OK )
			{
				if( (0 < Buf[ 0 ]) && (Buf[ 0 ] < 255) )
					TP.version = Buf[0];
			}
			break;
		case ILI2511:
			if( HAL_I2C_Mem_Read( &hi2c1, 0x82, 0x42, I2C_MEMADD_SIZE_8BIT, &Buf[0], 3, 10 ) == HAL_OK )
			{
				if( (0 < Buf[ 0 ]) && (Buf[ 0 ] < 255) )
					TP.version = Buf[0];
			}
			break;
		default:
			TP.type = NTP;
			return;
	}
}

#endif

#ifdef FT5426

bool STM32TouchController::FT5426_Read ( int32_t& x, int32_t& y )
{
		// +------+-----------+--------+--------+--------+--------+--------+--------+--------+--------+
		// | Addr |    Name   |   b7   |   b6   |   b5   |   b4   |   b3   |   b2   |   b1   |   b0   |
		// +------+-----------+--------+--------+--------+--------+--------+--------+--------+--------+
		// | 0x02 | Cur Point |      			  Number of touch points[7:0]  						  |
		// +------+-----------+-----------------+--------+--------+-----------------------------------+
		// | 0x03 | TOUCH1_XH | 1st Event Flag  |        |		  | 	1st Touch X Position[11:8]    |
		// +------+-----------+-----------------+--------+--------+-----------------------------------+
		// | 0x04 | TOUCH1_XL | 				  1st Touch X Position[7:0]							  |
		// +------+-----------+-----------------------------------------------------------------------+
		// | 0x05 | TOUCH1_YH | 	  1st Touch ID[3:0]			  |     1st Touch Y Position[11:8]    |
		// +------+-----------+-----------------------------------+-----------------------------------+
		// | 0x06 | TOUCH1_YL | 				  1st Touch Y Position[7:0]							  |
		// +------+-----------+-----------------------------------------------------------------------+

		#pragma pack( 1 )
		union{
			uint8_t Buf[ 5 ];
			struct{
				/*-------- Byte0 --------*/
				uint8_t point_number :8;
				/*-------- Byte1 --------*/
				uint8_t X_H :4;
				uint8_t :2;
				uint8_t Event :2;
				/*-------- Byte2 --------*/
				uint8_t X_L :8;
				/*-------- Byte3 --------*/
				uint8_t Y_H :4;
				uint8_t ID :4;
				/*-------- Byte4 --------*/
				uint8_t Y_L :8;
			}reg;
		}Data;
		#pragma pack( )

		if(HAL_I2C_Mem_Read( &hi2c1, 0x70, 0x02, I2C_MEMADD_SIZE_8BIT, &Data.Buf[0], 5, 10 ) != HAL_OK)
			return false;

		if( Data.reg.ID != 0 )
			return false;

		if( ( Data.reg.Event != 0x00 ) && ( Data.reg.Event != 0x02 ) )
			return false;

		uint32_t sx,sy;

		sx = ((Data.reg.X_H & 0x0F) << 8) | Data.reg.X_L;
		sy = ((Data.reg.Y_H & 0x0F) << 8) | Data.reg.Y_L;
		x = (sx * 1024) / TP.range.x;
		y = (sy * 600) / TP.range.y;

		return true;
}

#endif

#ifdef ILI2130

bool STM32TouchController::ILI2130_Read ( int32_t& x, int32_t& y )
{
	// I2C Packet Format
	// +-----------------+---------------- +---------------------------------+
	// | Number of Bytes |	 Description   |              Notes              |
	// +-----------------+-----------------+---------------------------------+
	// | 	   0		 |    Report ID    | I2C = 0x48                      |
	// +-----------------+-----------------+---------------------------------+
	// |       1~5       |     Point 1     | Please refer Report ID Format   |
	// +-----------------+-----------------+---------------------------------+
	// |       6~10      |     Point 2     | Please refer Report ID Format   |
	// +-----------------+-----------------+---------------------------------+
	// |       11~15     |     Point 3     | Please refer Report ID Format   |
	// +-----------------+-----------------+---------------------------------+
	// |       16~20     |     Point 4     | Please refer Report ID Format   |
	// +-----------------+-----------------+---------------------------------+
	// |       21~25     |     Point 5     | Please refer Report ID Format   |
	// +-----------------+-----------------+---------------------------------+
	// |       26~30     |     Point 6     | Please refer Report ID Format   |
	// +-----------------+-----------------+---------------------------------+
	// |       31~35     |     Point 7     | Please refer Report ID Format   |
	// +-----------------+-----------------+---------------------------------+
	// |       36~40     |     Point 8     | Please refer Report ID Format   |
	// +-----------------+-----------------+---------------------------------+
	// |       41~45     |     Point 9     | Please refer Report ID Format   |
	// +-----------------+-----------------+---------------------------------+
	// |       46~50     |     Point 10    | Please refer Report ID Format   |
	// +-----------------+-----------------+---------------------------------+
	// |       51~60     |     Reserve     |                                 |
	// +-----------------+-----------------+---------------------------------+
	// |       61        |  Contact Count  |                                 |
	// +-----------------+-----------------+---------------------------------+
	// |       62        |     Reserve     |                                 |
	// +-----------------+-----------------+---------------------------------+
	// |       63        |    Check Sum    |                                 |
	// +-----------------+-----------------+---------------------------------+

	// Report ID Format
	// +-----------------+-------------------------+------+------+------+------+------+------+------+------+
	// | Number of Bytes |	     Description       |  b7  |  b6  |  b5  |  b4  |  b3  |  b2  |  b1  |  b0  |
	// +-----------------+-------------------------+------+------+------+------+------+------+------+------+
	// |        1        | Tip Switch and Point ID |  0   | Tip  |                 Point ID                |
	// +-----------------+-------------------------+------+------+------+------+------+------+------+------+
	// |        2        |                         | X Position (LSB)                                      |
	// +-----------------+ X direction coordinate  +------+------+------+------+------+------+------+------+
	// |        3        |                         | X Position (MSB)                                      |
	// +-----------------+-------------------------+------+------+------+------+------+------+------+------+
	// |        4        |                         | Y Position (LSB)                                      |
	// +-----------------+ Y direction coordinate  +------+------+------+------+------+------+------+------+
	// |        5        |                         | Y Position (MSB)                                      |
	// +-----------------+-------------------------+------+------+------+------+------+------+------+------+

	#pragma pack( 1 )
	union{
		uint8_t Buf[ 64 ];
		struct{
			/*-------- Byte0 --------*/
			uint8_t Report_ID :8;
			/*-------- Byte1 to Byte50 --------*/
			struct{
				uint8_t Point_ID :6;
				uint8_t Tip :1;
				uint8_t :1;

				uint16_t x;

				uint16_t y;
			}point[ 10 ];
			/*-------- Byte51 to Byte60 --------*/
			uint8_t Reserve[ 10 ];
			/*-------- Byte61 --------*/
			uint8_t Contact_Count :8;
			/*-------- Byte62 --------*/
			uint8_t :8;
			/*-------- Byte63 --------*/
			uint8_t Check_Sum :8;
		}reg;
	}Data;
	#pragma pack( )

	if( HAL_I2C_Mem_Read( &hi2c1, 0x82, 0x10, I2C_MEMADD_SIZE_8BIT, &Data.Buf[0], 64, 10 ) != HAL_OK )
	{
		HAL_I2C_DeInit(&hi2c1);
		HAL_I2C_Init(&hi2c1);
		HAL_GPIO_WritePin(CTP_RST_GPIO_Port, CTP_RST_Pin, GPIO_PIN_RESET);
		HAL_Delay( 20 );
		HAL_GPIO_WritePin(CTP_RST_GPIO_Port, CTP_RST_Pin, GPIO_PIN_SET);
		HAL_Delay( 1000 );
		return false;
	}

	if( Data.reg.Report_ID != 0x48 )
		return false;

	if( Data.reg.point[0].Tip == 0 ) // Tip switch
		return false;

	if( Data.reg.point[0].Point_ID != 0x00 ) // Point ID = 0
		return false;

	x = ( Data.reg.point[0].x * 1024 ) / TP.range.x;
	y = ( Data.reg.point[0].y * 600 ) / TP.range.y;

	return true;
}

#endif

#ifdef ILI2511

bool STM32TouchController::ILI2511_Read ( int32_t& x, int32_t& y )
{
	// +----------+--------------+------+---------------+------+------+------+------+------+------+------+
	// | CMD Code |     Name     | Note |      b7       |  b6  |  b5  |  b4  |  b3  |  b2  |  b1  |  b0  |
	// +----------+--------------+------+---------------+------+------+------+------+------+------+------+
	// |          | Touch        |      | 0: No touch	                                                 |
	// |          | Information  |      | 1: Last Report as ID0 to ID5(include release ststus)           |
	// |          |              |      | 1: Last Report as ID0 to ID5(include release ststus)           |
	// |          |              +------+---------------+------+-----------------------------------------+
	// |          |              | ID0  | 0: Touch Off  |  0   | X_High direction coordinate             |
	// |          |              |      | 1: Touch Down |      |                                         |
	// |          |              |      +---------------+------+-----------------------------------------+
	// |          |              |      | X_Low direction coordinate                                     |
	// |          |              |      +---------------+------+-----------------------------------------+
	// |          |              |      |       0       |  0   | Y_High direction coordinate             |
	// |          |              |      +---------------+------+-----------------------------------------+
	// |          |              |      | Y_Low direction coordinate                                     |
	// |          |              +------+----------------------------------------------------------------+
	// |          |              |   .  |                                .                               |
	// |          |              |   .  |                                .                               |
	// |          |              |   .  |                                .                               |
	// |          |              |   .  |                                .                               |
	// |          |              +------+---------------+------+-----------------------------------------+
	// |          |              | ID5  | 0: Touch Off  |  0   | X_High direction coordinate             |
	// |          |              |      | 1: Touch Down |      |                                         |
	// |          |              |      +---------------+------+-----------------------------------------+
	// |          |              |      | X_Low direction coordinate                                     |
	// |          |              |      +---------------+------+-----------------------------------------+
	// |          |              |      |       0       |  0   | Y_High direction coordinate             |
	// |          |              |      +---------------+------+-----------------------------------------+
	// |          |              |      | Y_Low direction coordinate                                     |
	// +----------+--------------+------+----------------------------------------------------------------+

	#pragma pack( 1 )
	union{
		uint8_t Buf[ 31 ];
		struct{
			/*-------- Byte0 --------*/
			uint8_t Packet_Number :8;
			/*-------- Byte1 to Byte50 --------*/
			struct{
				uint8_t X_H:6;
				uint8_t :1;
				uint8_t Touch_Status :1;

				uint8_t X_L :8;

				uint8_t Y_H:6;
				uint8_t :2;

				uint8_t Y_L :8;

				uint8_t Touch_Pressure :8;
			}point[ 6 ];
		}reg;
	}Data;
	#pragma pack( )

	uint32_t sx,sy;

	switch( TP.version )
	{
		case 2:
			break;
		case 3:
			if( HAL_I2C_Mem_Read( &hi2c1, 0x82, 0x10, I2C_MEMADD_SIZE_8BIT, &Data.Buf[0], 31, 10 ) != HAL_OK )
				return false;

			if( Data.reg.point[0].Touch_Status != 1 )
				return false;

			sx = ( (Data.reg.point[0].X_H & 0x3F) << 8 ) | Data.reg.point[0].X_L;
			sy = ( (Data.reg.point[0].Y_H & 0x3F) << 8 ) | Data.reg.point[0].Y_L;

			x = ( sx * 1024 ) / TP.range.x;
			y = ( sy * 600 ) / TP.range.y;

			return true;
		default:
			break;
	}
	return false;
}

#endif
/* USER CODE END STM32TouchController */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
