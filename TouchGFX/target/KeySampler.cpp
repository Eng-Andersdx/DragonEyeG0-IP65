/**
  ******************************************************************************
  * @file    KeySampler.cpp
  * @author  Anders Engineering Team
  * @version 1.0
  * @brief   This file provides a set of functions needed to sense rotation of rotary
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
 * 1.0     | 2025-06-10 | R.Gopan   | Initial BSP
 * -----------------------------------------------------------------------------
 * Notes:
 * - Always increment version when making manual changes.
 * - Describe clearly what was changed in each version.
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "KeySampler.hpp"
#include "touchgfx/hal/Buttons.hpp"
#include "string.h"
#include "main.h"
#include "bsp.h"
#include <touchgfx/hal/GPIO.hpp>
#include "touchgfx/hal/OSWrappers.hpp"
#include "touchgfx/hal/HAL.hpp"

static TIM_TypeDef* const LINE_TIMER = TIM17;
volatile int val;
extern volatile uint8_t Touch_Event_Flag;

#if __KEYSAMPLER_HPP != 0100	/* BSP Rotary REVISION v.1.0 */
#error Wrong API version file (KeySampler.hpp).
#endif


/** @defgroup DragonEYE_ENCODER_Private_Defines DragonEYE Encoder Private Types Defines
  * @{
  */
#define ENCODER_KEYA_GPIO_CLK_ENABLE()			__HAL_RCC_GPIOA_CLK_ENABLE()
#define ENCODER_KEYA_GPIO_CLK_DISABLE()			__HAL_RCC_GPIOA_CLK_DISABLE()
#define ENCODER_KEYB_GPIO_CLK_ENABLE()			__HAL_RCC_GPIOA_CLK_ENABLE()
#define ENCODER_KEYB_GPIO_CLK_DISABLE()			__HAL_RCC_GPIOA_CLK_DISABLE()
#define ENCODER_PUSH_BUTTON_GPIO_CLK_ENABLE()	__HAL_RCC_GPIOA_CLK_ENABLE()
#define ENCODER_PUSH_BUTTON_GPIO_CLK_DISABLE()	__HAL_RCC_GPIOA_CLK_DISABLE()

#define LATCH0 0
#define LATCH3 3
/**
  * @}
  */

/** @defgroup DragonEYE_ENCODER_Config_Defines DragonEYE Encoder Configuration Defines
  * @{
  */
	/* Specify the status of KeyA and Key B terminal on the dent position for the specific encoder used
	* If no #define is selected, then it is assumed both KeyA and KeyB are in High states on each dent position
	*/
#define LOW_STATE_DENTS
#define BOTH_STATE_DENTS
/**
  * @}
  */


encLatchMode_t xEncoderLatchMode = encLatchMode_TWO03;
volatile uint8_t	vui8EncoderState_Prev;        // old internal position
volatile long		vlEncoderPosition_Int;        // internal position, 4 times position_ext
//Actual Encoder States
signed long			lEncoderPosition = 0;
static uint8_t             clockwise_counter=0;
static uint8_t             anti_clockwise_counter=0;
static uint8_t             push_counter=0;
volatile uint32_t pushStartTick = 0;
volatile uint32_t pushDuration  = 0;
#define LONG_PRESS_THRESHOLD 1000
uint8_t				ui8EncoderPushed = 0;

static uint8_t lastABState = 0;
/**
  * @}
  */



/** @defgroup De_Lite_ENCODER_Private_Variables DragonEYE Encoder Private Variables
  * @{
  */
GPIO_TypeDef* ENCODER_PORT[3] = {HALL_S1_GPIO_Port,
								 HALL_S2_GPIO_Port,
								 CTP_INT_GPIO_Port};

const uint16_t ENCODER_PIN[3] = {HALL_S1_Pin,
								 HALL_S2_Pin,
								 CTP_INT_Pin};

const uint16_t ENCODER_IRQn[3] = {EXTI4_15_IRQn,
								  EXTI4_15_IRQn,
								  EXTI0_1_IRQn};

#ifdef LOW_STATE_DENTS

#define RISING_EDGE 0
#define	FALLING_EDGE 1

#else

#define FALLING_EDGE 0
#define	RISING_EDGE 1

#endif




using namespace touchgfx;


extern "C" void touchgfxSignalVSync(void);
extern void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
extern volatile uint16_t tearingEffectCount;



using namespace touchgfx;
uint32_t PrevStata_BLeft=1;
uint32_t PrevStata_BRight=1;
uint32_t PrevStata_BPush=1;

/**
  * @brief  Initialise the Encoder PushButton and the Rotation
  */
void KeySampler::init()
{
	BSP_PB_Init(BUTTON_LEFT, BUTTON_MODE_EXTI);
	BSP_PB_Init(BUTTON_RIGHT, BUTTON_MODE_EXTI);
	BSP_PB_Init(BUTTON_PUSH, BUTTON_MODE_EXTI);
}

/**
  * @brief  Detects the clockwise/Anti-clockwise or push.
  * @param  key: Touchgfx variable for the rotation/ push button
  * @ret         Boolean
  */
bool KeySampler::sample(uint8_t& key)
{
	 uint32_t result = 0;

	 result=BSP_PB_GetState(BUTTON_LEFT);
	 if(result==0 && PrevStata_BLeft != result){
		 PrevStata_BLeft = result;
		 BSP_ENC_ResetRotationValue();
		 key=0;
	     return true;
	 }
	 PrevStata_BLeft = result;

	 result=BSP_PB_GetState(BUTTON_RIGHT);
	 if(result==0 && PrevStata_BRight != result){
	 	key=1;
	   	PrevStata_BRight = result;
	   	BSP_ENC_ResetRotationValue();
	    return true;
	 }
	 PrevStata_BRight = result;


	 result=BSP_PB_GetState(BUTTON_PUSH);
	 if(result==0 && PrevStata_BPush != result){
	    PrevStata_BPush = result;
	    key=2;
	    return true;
	 }
	 PrevStata_BPush = result;

	 return false;
}

/**
  * @brief  Configures Encoder GPIO and EXTI Line.
  * @param  Encoder_Mode: Encoder sensing mode
  *          This parameter can be one of the following values:
  *            @arg  ENCODER_MODE_POLL: Encoder will be used as simple IO and checked in polling mode
  *            @arg  ENCODER_MODE_EXTI: Encoder will be connected to EXTI lines and checked in Interrupt mode
  *                                    with interrupt generation capability
  */
void BSP_ENC_Init(EncoderMode_TypeDef Encoder_Mode)
{
	GPIO_InitTypeDef gpio_init_structure;

	/* Enable the BUTTON Port clock */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	if(Encoder_Mode == ENCODER_MODE_POLL)
	{
		/* Configure KeyA pin as input */
		gpio_init_structure.Pin = ENCODER_PIN[KEY_A];
		gpio_init_structure.Mode = GPIO_MODE_INPUT;
		gpio_init_structure.Pull = GPIO_PULLUP;
		HAL_GPIO_Init(ENCODER_PORT[KEY_A], &gpio_init_structure);

		/* Configure KeyB pin as input */
		gpio_init_structure.Pin = ENCODER_PIN[KEY_B];
		gpio_init_structure.Mode = GPIO_MODE_INPUT;
		gpio_init_structure.Pull = GPIO_PULLUP;
		HAL_GPIO_Init(ENCODER_PORT[KEY_B], &gpio_init_structure);

		/* Configure Push Button pin as input */
		gpio_init_structure.Pin = ENCODER_PIN[PUSH_BUTTON];
		gpio_init_structure.Mode = GPIO_MODE_INPUT;
		gpio_init_structure.Pull = GPIO_PULLUP;
		HAL_GPIO_Init(ENCODER_PORT[PUSH_BUTTON], &gpio_init_structure);
	}

	if(Encoder_Mode == ENCODER_MODE_EXTI)
	{
		/* Configure KeyA pin as input with External interrupt */
		gpio_init_structure.Pin = ENCODER_PIN[KEY_A];
		gpio_init_structure.Mode = GPIO_MODE_IT_RISING_FALLING;
		gpio_init_structure.Pull = GPIO_PULLUP;
		HAL_GPIO_Init(ENCODER_PORT[KEY_A], &gpio_init_structure);
		/* Enable and set KeyA EXTI Interrupt priority */
		HAL_NVIC_SetPriority((IRQn_Type)(ENCODER_IRQn[KEY_A]), 0x02, 0x00);
		HAL_NVIC_EnableIRQ((IRQn_Type)(ENCODER_IRQn[KEY_A]));

		/* Configure KeyB pin as input with External interrupt */
		gpio_init_structure.Pin = ENCODER_PIN[KEY_B];
		gpio_init_structure.Mode = GPIO_MODE_IT_RISING_FALLING;
		gpio_init_structure.Pull = GPIO_PULLUP;
		HAL_GPIO_Init(ENCODER_PORT[KEY_B], &gpio_init_structure);
		/* Enable and set KeyB EXTI Interrupt priority */
		HAL_NVIC_SetPriority((IRQn_Type)(ENCODER_IRQn[KEY_B]), 0x02, 0x00);
		HAL_NVIC_EnableIRQ((IRQn_Type)(ENCODER_IRQn[KEY_B]));

		/* Configure Push Button pin as input with External interrupt */
		gpio_init_structure.Pin = ENCODER_PIN[PUSH_BUTTON];
		gpio_init_structure.Mode = GPIO_MODE_IT_RISING_FALLING;
		gpio_init_structure.Pull = GPIO_PULLUP;
		HAL_GPIO_Init(ENCODER_PORT[PUSH_BUTTON], &gpio_init_structure);
		/* Enable and set Push Button EXTI Interrupt priority */
		HAL_NVIC_SetPriority((IRQn_Type)(ENCODER_IRQn[PUSH_BUTTON]), 0x02, 0x00);
		HAL_NVIC_EnableIRQ((IRQn_Type)(ENCODER_IRQn[PUSH_BUTTON]));
	}

	//Reset all internal counters
	vui8EncoderState_Prev = 0;
	vlEncoderPosition_Int = 0;
	lEncoderPosition = 0;
	ui8EncoderPushed = 0;
}

/**
  * @brief  Configures button GPIO and EXTI Line.
  * @param  Button: Button to be configured
  *          This parameter can be one of the following values:
  *            @arg  BUTTON_WAKEUP: Wakeup Push Button
  *            @arg  BUTTON_USER: User Push Button
  * @param  Button_Mode: Button mode
  *          This parameter can be one of the following values:
  *            @arg  BUTTON_MODE_GPIO: Button will be used as simple IO
  *            @arg  BUTTON_MODE_EXTI: Button will be connected to EXTI line
  *                                    with interrupt generation capability
  */
void BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef Button_Mode)
{
	if(Button_Mode==BUTTON_MODE_EXTI){
		BSP_ENC_Init(ENCODER_MODE_EXTI);
	} else {
		// Hardcoded option for now...
		BSP_ENC_Init(ENCODER_MODE_EXTI);
	}
}


/**
  * @brief  This function resets the encoder position counts.
  * @param  None
  * @retval None
  */
void BSP_ENC_ResetRotationValue(void)
{
	vui8EncoderState_Prev = 0;
	vlEncoderPosition_Int = 0;
	lEncoderPosition = 0;
	ui8EncoderPushed = 0;
}

/**
 * @brief  This function report encoder rotation position
 * @param  None
 * @retval The Encoder absolute rotation value (integer limited)
 */
signed long BSP_ENC_GetRotationValue(void)
{
	return lEncoderPosition;
}

/**
 * @brief  This function report the number of clockwise rotations of the encoder
 * @param  None
 * @retval The clockwise_counter
 */
uint8_t BSP_ENC_GetClockWiseRotation(void){
	return clockwise_counter;
}

/**
 * @brief  This function report the number of pushes in the encoder
 * @param  None
 * @retval The push_counter
 */
uint8_t BSP_ENC_GetPushCounter(void){
	return push_counter;
}

/**
 * @brief  This function report the number of anti clockwise rotations of the encoder
 * @param  None
 * @retval The anti_clockwise_counter
 */
uint8_t BSP_ENC_GetAntiClockWiseRotation(void){
	return anti_clockwise_counter;
}

/**
  * @brief  Returns the Push button state at any time.
  * @param  None
  * @retval The Button GPIO pin value
  */
EncoderPushValue_TypeDef BSP_ENC_GetPushBtnState(void)
{
	if(ui8EncoderPushed){
		ui8EncoderPushed = 0;
		return ENC_PB_PUSHED;
	}
	return ENC_PB_RELEASED;
}

/**
 * @brief  Returns the selected button state.
 * @param  Button: Button to be checked
 *          This parameter can be one of the following values:
 *            @arg  BUTTON_WAKEUP: Wakeup Push Button
 *            @arg  BUTTON_USER: User Push Button
 * @retval The Button GPIO pin value
 */
uint32_t BSP_PB_GetState(Button_TypeDef Button){

	switch(Button){
	case BUTTON_LEFT:
		if(anti_clockwise_counter){
			anti_clockwise_counter--;
			return PB_SET;
		}
		else{
			return PB_RESET;
		}
		break;
	case BUTTON_RIGHT:
		if(clockwise_counter){
			clockwise_counter--;
			return PB_SET;
		}
		else{
			return PB_RESET;
		}
		break;
	case BUTTON_PUSH:
		if(ui8EncoderPushed==ENC_PB_PUSHED){
			return PB_SET;
		}
		else{
			return PB_RESET;
		}
	default:
		return PB_RESET;
	}
}


/**
 * @brief Callback when their is a rising edge of GPIO_Pin
 * @param uint16_t GPIO_Pin: The pin number corresponding to the rising edge
 */
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == DISPLAY_TEARING_EFFECT_Pin)
    {
    	LL_TIM_DisableCounter(TIM17);  // Stop counting
    	LL_TIM_SetCounter(TIM17, 0);   // Reset CNT register to 0
        tearingEffectCount++;
        touchgfx::HAL::getInstance()->vSync();
        touchgfx::OSWrappers::signalVSync();
        GPIO::set(GPIO::VSYNC_FREQ);

    }
    if((GPIO_Pin == GPIO_PIN_6) | (GPIO_Pin == GPIO_PIN_7) | (GPIO_Pin == CTP_INT_Pin)){
        	HAL_GPIO_EXTI_Callback(GPIO_Pin);
     }

}


/**
 * @brief Callback when their is a falling edge of the GPIO_Pin
 * @param uint16_t GPIO_Pin: The pin number corresponding to the falling edge
 */
void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin){


	if((GPIO_Pin == GPIO_PIN_6) | (GPIO_Pin == GPIO_PIN_7) | (GPIO_Pin == CTP_INT_Pin)){
    	HAL_GPIO_EXTI_Callback(GPIO_Pin);
    }
	else if (GPIO_Pin == DISPLAY_TEARING_EFFECT_Pin )
	  {
	    LL_TIM_EnableCounter(LINE_TIMER);
	    GPIO::clear(GPIO::VSYNC_FREQ);
	  }
}

/* encoder interrupt callback */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	/* KeyA falling edge interrupt generated */
	if ((GPIO_Pin==ENCODER_PIN[KEY_A]) || (GPIO_Pin==ENCODER_PIN[KEY_B]))
	{
		static bool valid =true;

	    uint8_t A = HAL_GPIO_ReadPin(ENCODER_PORT[KEY_A], ENCODER_PIN[KEY_A]) ? 1 : 0;
	    uint8_t B = HAL_GPIO_ReadPin(ENCODER_PORT[KEY_B], ENCODER_PIN[KEY_B]) ? 1 : 0;

	    uint8_t currentState = (A << 1) | B;
	    uint8_t transition = (lastABState << 2) | currentState;

	    // Decode rotation direction
	    if(valid){
			switch (transition)
			{
				case 0b0100:	lEncoderPosition--;		anti_clockwise_counter++; valid=!valid;	break;
				case 0b0111:    lEncoderPosition++;		clockwise_counter++; valid=!valid;		break;
				case 0b1000:	lEncoderPosition++;		clockwise_counter++; valid=!valid;	break;
				case 0b1011:	lEncoderPosition--;		anti_clockwise_counter++; valid=!valid;	break;
				default:		break;
				}
	    } else{
	    	valid=!valid;
	    }
	    lastABState = currentState;
	}

	if(GPIO_Pin==ENCODER_PIN[PUSH_BUTTON]) {
			Touch_Event_Flag = 1;
			ui8EncoderPushed = ENC_PB_PUSHED;
	}
}
