/**
  ******************************************************************************
  * @file    HapticLogic.hpp
  * @author  Anders Engineering Team
  * @version 1.0
  * @brief   This file provides a set of functions needed to control the duration of haptic feedback
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
 * 1.0     |2026-04-03  | R.Gopan       | Haptic control
 * -----------------------------------------------------------------------------
 * Notes:
 * - Always increment version when making manual changes.
 * - Describe clearly what was changed in each version.
 ******************************************************************************/

/* File Info : -----------------------------------------------------------------
                                   User NOTES
1. Header description:
----------------------
   - This header is used to control the duration for haptic feedback.
     This header can be included in the TouchGFX .cpp/.hpp files to trigger the duration of haptic feeback
     thus providing GUI to decide on the haptic feeback

2. How To use this header:
--------------------------
  + Initialization steps:
     o Include the header to .cpp/.hpp file

  +  How to use the header
     o Trigger the haptic sensor by using trigger(uint16_t durationMs) function.
     o Call the update() function inside the handleTickEvent() function to stop the
       haptic sensor after the particular duration defined in trigger function

------------------------------------------------------------------------------*/

#ifndef GUI_INCLUDE_GUI_COMMON_HAPTICLOGIC_HPP_
#define GUI_INCLUDE_GUI_COMMON_HAPTICLOGIC_HPP_

#pragma once
#include <stdint.h>
#ifdef KNOB_INTERFACE
#include "knob_interface.hpp"            // knobHapticOn() / knobHapticOff(), board only
#endif

class HapticLogic
{
public:
    void trigger(uint16_t durationMs)
    {
#ifdef KNOB_INTERFACE
        knobHapticPulse(durationMs);
#else
        (void)durationMs;
#endif
    }

    void update() {}       // timing is on the board side now (knobTick in KeySampler)
};


#endif /* GUI_INCLUDE_GUI_COMMON_HAPTICLOGIC_HPP_ */
