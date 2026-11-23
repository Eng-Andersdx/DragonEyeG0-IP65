#include "main.h"
#include "knob_interface.hpp"
#include "dragoneye_leds.h"
#include "de_lite_haptic_sensor.h"

/* Haptic pulse state (board side) */
static uint32_t hapticOffAt  = 0;    /* HAL tick when the haptic must switch off */
static uint8_t  hapticActive = 0;    /* 1 while a pulse is running */

static USART_TypeDef* const UART_COM = USART1;

void knobUartSendByte(uint8_t data)
{
}

void knobUartSendBytes(uint8_t* data, uint16_t size)
{
}

void knobUartSendString(uint8_t* data)
{

}

uint8_t knobUartReceiveByte(uint8_t* data)
{
	return 0;
}

void knobSetAmbientLightRGB(uint8_t red, uint8_t green, uint8_t blue)
{
	red = (red * 100) / 255;
	green = (green * 100) / 255;
	blue = (blue * 100) / 255;
	BSP_LED_SetRGBIntensity(red, green, blue);
}


void knobHapticPulse(uint16_t ms)
{
    BSP_HAPTIC_ON();
    hapticOffAt  = HAL_GetTick() + ms;
    hapticActive = 1;
}

void knobTick(void)
{
    if (hapticActive && (int32_t)(HAL_GetTick() - hapticOffAt) >= 0) {
        BSP_HAPTIC_OFF();
        hapticActive = 0;
    }
}

void knobSetBacklight(uint8_t percent)
{
	if (percent > 100) percent = 100;
	BSP_BL_SetIntensity(percent);
}
