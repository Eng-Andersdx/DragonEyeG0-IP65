/**
  ******************************************************************************
  * @file    KeySampler.hpp
  * @author  Anders Engineering Team
  * @version 1.0
  * @brief   This file contains definitions for DragonEYE Rotary hardware
  *          resources.
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


#ifndef TARGET_KEYSAMPLER_HPP_
#define TARGET_KEYSAMPLER_HPP_

/* Includes ------------------------------------------------------------------*/
#include <platform/driver/button/ButtonController.hpp>


#ifndef __KEYSAMPLER_HPP
#define __KEYSAMPLER_HPP  0100 /* BSP Rotary API REVISION v.1.0 */

/** @brief EncoderPushValue_TypeDef
  *  Anders DVK board Encoders Push Button definition.
  */
typedef enum
{
	ENC_PB_RELEASED = 0,
	ENC_PB_PUSHED = !ENC_PB_RELEASED,
	ENC_PB_LONG_PRESS
	//ENC_PB_DOUBLE_CLICK
} EncoderPushValue_TypeDef;


/** @brief Button_TypeDef
  *  Anders DVK board Encoders GPIO definitions.
  */
typedef enum
{
	KEY_A = 0,
	KEY_B = 1,
	PUSH_BUTTON = 2
} Encoder_GPIO_TypeDef;


/** @brief EncoderMode_TypeDef
  *  Anders DVK board Encoder Modes definitions.
  */
typedef enum
{
	ENCODER_MODE_POLL = 0,
	ENCODER_MODE_EXTI = 1
} EncoderMode_TypeDef;


/**
  * encLatchMode_FOUR0 - Use this mode when both encoder output signals are always LOW in latch position.
  * encLatchMode_FOUR3 - Use this mode when both encoder output signals are always HIGH in latch position.
  * encLatchMode_TWO03 - Use this mode when both encoder output signals are LOW or HIGH in latch position.
  */
typedef enum
{
	encLatchMode_FOUR3 = 1,
	encLatchMode_FOUR0 = 2,
	encLatchMode_TWO03 = 3
} encLatchMode_t;

/** @brief ButtonMode_TypeDef
  *  Anders EVK board Buttons definitions.
  */
typedef enum
{
  BUTTON_MODE_GPIO = 0,
  BUTTON_MODE_EXTI = 1
} ButtonMode_TypeDef;

/** @brief Button_TypeDef
  *  Anders EVK board Buttons definitions.
  */
typedef enum
{
  BUTTON_LEFT = 0,
  BUTTON_RIGHT = 1,
  BUTTON_PUSH = 2
} Button_TypeDef;


/** @defgroup DELite_ENCODER_Exported_Constants DELite Encoder Exported Constants
  * @{
  */
#define DOUBLE_COUNT_ENCODER 0


EncoderPushValue_TypeDef	BSP_ENC_GetPushBtnState(void);
void 						BSP_ENC_Init(EncoderMode_TypeDef Encoder_Mode);
void                        BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef Button_Mode);
uint32_t                    BSP_PB_GetState(Button_TypeDef Button);
void						BSP_ENC_ResetRotationValue(void);


namespace touchgfx
{
class KeySampler : public ButtonController
{
public:
    KeySampler()
    {
        init();
    }
    virtual ~KeySampler() {}

    /**
     * Initializes keySampler
     */
    virtual void init();

    /**
     * Sample external key events.
     * @param key Output parameter that will be set to the key value if a keypress was detected.
     *
     * @return True if a keypress was detected and the "key" parameter is set to a value.
     */
    virtual bool sample(uint8_t& key);

private:
    uint8_t previousState;
};
}

#endif /* TARGET_KEYSAMPLER_HPP_ */
#endif
