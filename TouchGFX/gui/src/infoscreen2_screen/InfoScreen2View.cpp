#include <gui/infoscreen2_screen/InfoScreen2View.hpp>
#include "DE_Lite/dragoneye_leds.h"
#include "math.h"

extern TIM_HandleTypeDef htim1;

InfoScreen2View::InfoScreen2View()
	: infoScreenCallback(this, &InfoScreen2View::InfoScreenClickHandler)
	, pressTicks(0)
	, isPressed(false)
	, longPressTrigerred(false)
	, accumulatedDx(0)
{

}

void InfoScreen2View::setupScreen()
{
    InfoScreen2ViewBase::setupScreen();
    backGroundBox.setClickAction(infoScreenCallback);
}

void InfoScreen2View::tearDownScreen()
{
    InfoScreen2ViewBase::tearDownScreen();
}

void InfoScreen2View::infoScreenLedOn(){
	BSP_LED_SetColor(RGB_CYAN);
}


void InfoScreen2View::handleTickEvent(){
	  if (isPressed && !longPressTrigerred){
	        	pressTicks++;
	      if (pressTicks > 60){
	       	   longPressTrigerred= true;
	       	   isPressed=false;
	       	   application().gotoMenuScreenScreenNoTransition();
	       	   pressTicks=0;
	      }
	  }
}

void InfoScreen2View::InfoScreenClickHandler(const Box& box, const ClickEvent& evt){
	if(&box == &backGroundBox){
		if (evt.getType() == touchgfx::ClickEvent::PRESSED)
		{
			pressTicks=0;
			isPressed = true;
			longPressTrigerred = false;
			accumulatedDx=0;
		}
	    else if (evt.getType() == touchgfx::ClickEvent::RELEASED)
		{
	    	isPressed = false;
		}
	}
}
