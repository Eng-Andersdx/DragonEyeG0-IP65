#include <gui/batteryscreen_screen/BatteryScreenView.hpp>
//#include "DE_Lite/dragoneye_leds.h"
#include <gui/common/LedColors.hpp>      /* 2026-11-23: LED colours via presenter, no BSP include */
#include <stdlib.h>   /* abs() for tap/drag detection */

BatteryScreenView::BatteryScreenView()
	: batteryImageCallback(this, &BatteryScreenView::BatteryScreenClickHandler)
	, pressTicks(0)
	, isPressed(false)
	, longPressTrigerred(false)
	, tapStartX(0)
	, tapStartY(0)
	, tapMoved(true)
{

}

void BatteryScreenView::setupScreen()
{
    BatteryScreenViewBase::setupScreen();

    fillBox.setClickAction(batteryImageCallback);

//presenter->setAmbientLightRGB(false, true, false);
    presenter->setAmbientLightRGB(LED_GREEN);

    updateBatteryIndicator();

    // Hide some widget to shorten the initial screen render time
    chargeIconImage.setVisible(false);
    chargeTextArea.setVisible(false);
    percentTextArea.setVisible(false);
}

void BatteryScreenView::tearDownScreen()
{
    BatteryScreenViewBase::tearDownScreen();
}

void BatteryScreenView::handleTickEvent()
{
    BatteryScreenViewBase::handleTickEvent();

//    if(handleLongPressLogic(pressTicks, isPressed, longPressTrigerred)){
//    	application().gotoMenuScreenScreenNoTransition();
//    }
    /* 2026-11-13: long press removed (tap -> key 53 from the touch driver). Previous: */
//    if (isPressed && !longPressTrigerred){
//           	pressTicks++;
//              if (pressTicks > 60){
//           	   longPressTrigerred= true;
//           	   isPressed=false;
//           	   application().gotoMenuScreenScreenNoTransition();
//           	   pressTicks=0;
//              }
//            }


    // Set the hidden widgets visible on the first tick
    if (!chargeTextArea.isVisible())
    {
        chargeIconImage.setVisible(true);
        chargeIconImage.invalidate();
        chargeTextArea.setVisible(true);
        chargeTextArea.invalidate();
        percentTextArea.setVisible(true);
        percentTextArea.invalidate();
    }

    updateBatteryIndicator();
}


void BatteryScreenView::BatteryScreenClickHandler(const Box& batteryImage, const ClickEvent& evt){
	if(&batteryImage == &fillBox){
//		handlePressLogic(evt, pressTicks, isPressed,longPressTrigerred);

		if (evt.getType() == touchgfx::ClickEvent::PRESSED)
		{
			isPressed = true;
			longPressTrigerred = false;
		 }
		 else if (evt.getType() == touchgfx::ClickEvent::RELEASED)
		 {
		    isPressed = false;
		 }
	}
}



void BatteryScreenView::handleClickEvent(const ClickEvent& evt)
{
    BatteryScreenViewBase::handleClickEvent(evt);

    if (evt.getType() == ClickEvent::PRESSED)
    {
        tapStartX = evt.getX();
        tapStartY = evt.getY();
        tapMoved  = false;
    }
    else if (evt.getType() == ClickEvent::RELEASED && !tapMoved)
    {
        tapMoved = true;
//      handleKeyEvent(53);   /* 2026-11-13: tap -> key 53 now comes from the touch driver */
    }
}

void BatteryScreenView::handleDragEvent(const DragEvent& evt)
{
    BatteryScreenViewBase::handleDragEvent(evt);

    if (abs(evt.getNewX() - tapStartX) > 5 || abs(evt.getNewY() - tapStartY) > 5)
    {
        tapMoved = true;          /* it's a drag, not a tap */
    }
}

void BatteryScreenView::updateBatteryIndicator()
{
    const uint16_t newBatteryLevel = presenter->getBatteryLevel();

    // Update the graphics to reflect the current battery level
    const int16_t newBatteryFillLevel = HAL::DISPLAY_HEIGHT - (newBatteryLevel * HAL::DISPLAY_HEIGHT) / 1000;
    if (newBatteryFillLevel != oldBatteryFillLevel)
    {
        waveAnimatedImage.invalidate();
        waveAnimatedImage.setY(newBatteryFillLevel);
        backgroundTiledImage.setHeight(newBatteryFillLevel);
        waveAnimatedImage.invalidate();

        oldBatteryFillLevel = newBatteryLevel;
    }

    // Update the text field to reflect the current battery level
    const uint16_t newBatteryPercentage = (newBatteryLevel * 100) / 1000;
    if (newBatteryPercentage != oldBatteryPercentage)
    {
        Unicode::snprintf(chargeTextAreaBuffer, CHARGETEXTAREA_SIZE, "%d", newBatteryPercentage);
        chargeTextArea.invalidate();

        oldBatteryPercentage = newBatteryPercentage;
    }
}

void BatteryScreenView::ChargeMenuLedOn(){
//	void BSP_LED_AllOff();
//	BSP_LED_SetColor(RGB_GREEN);
	presenter->setAmbientLightRGB(LED_GREEN);
}
