#include <gui/volumescreen_screen/VolumeScreenView.hpp>
#include "dragoneye_leds.h"
#include "math.h"

extern TIM_HandleTypeDef htim1;

using namespace touchgfx;

// Use TouchGFX' own PI to avoid ambiguity
static const float TWO_PI       = 2.0f * touchgfx::PI;


static const float SELECT_ANGLE = -touchgfx::PI / 2.0f;


VolumeScreenView::VolumeScreenView()
	: volumeImageProgressCallback(this, &VolumeScreenView::VolumeScreenClickHandler)
	, pressTicks(0)
	, isPressed(false)
	, longPressTrigerred(false)
	, dragged(false)
	, lastAngle(0.f)
	, accumulatedAngle(0.f)
	, centerX(120.f)
	, centerY(120.f)
{

}

void VolumeScreenView::setupScreen()
{
    VolumeScreenViewBase::setupScreen();

    circleProgress.setClickAction(volumeImageProgressCallback);

    presenter->setAmbientLightRGB(false, false, true);

    bigNumberCustomContainer1.updateValue(presenter->getVolume());

    // Setup the intro animation
    circleProgress.updateValue(0, 0);
    isAnimating = true;

    // Hide the bug text area to shorten the initial screen rendertime
    bigNumberCustomContainer1.setVisible(false);
}

void VolumeScreenView::tearDownScreen()
{
    VolumeScreenViewBase::tearDownScreen();
}

void VolumeScreenView::handleTickEvent()
{
    VolumeScreenViewBase::handleTickEvent();

    haptic.update();

    if (isPressed && !longPressTrigerred && !dragged){
       	pressTicks++;
          if (pressTicks > 60){
       	   longPressTrigerred=true;
       	   isPressed=false;
       	   application().gotoMenuScreenScreenNoTransition();
       	   pressTicks=0;
          }
    }

    // Set the big number text area visble on first tick
    if (!bigNumberCustomContainer1.isVisible())
    {
        bigNumberCustomContainer1.setVisible(true);
        bigNumberCustomContainer1.invalidate();
    }

    // Create the intro animation by increasing the displayed progress until it reaches the actual value
    else if (isAnimating)
    {
        int minValue, maxValue;
        circleProgress.getRange(minValue, maxValue);
        const uint16_t currentProgress = circleProgress.getProgress(maxValue - minValue);
        if (currentProgress < presenter->getVolume())
        {
            circleProgress.updateValue(currentProgress + 1, 0);
        }
        else
        {
            isAnimating = false;
        }
    }
}

void VolumeScreenView::VolumeScreenClickHandler(const CircleProgress& volumeProgress, const ClickEvent& evt){
	if(&volumeProgress == &circleProgress){
		if (evt.getType() == touchgfx::ClickEvent::PRESSED)
		    {
				lastAngle = angleFromPoint(evt.getX(), evt.getY());
				pressTicks =0;
		        isPressed = true;
		        longPressTrigerred = false;
		    }
		    else if (evt.getType() == touchgfx::ClickEvent::RELEASED)
		    {
		        isPressed = false;
		    }
	}
}

float VolumeScreenView::angleFromPoint(int16_t x, int16_t y) const
{
    float dx = float(x) - centerX;
    float dy = float(y) - centerY;
    return atan2f(dy, dx);   // [-pi..pi], 0 at right
}


float VolumeScreenView::normalize(float a) const
{
    while (a >  touchgfx::PI)  a -= TWO_PI;
    while (a < -touchgfx::PI)  a += TWO_PI;
    return a;
}


void VolumeScreenView::handleDragEvent(const DragEvent& evt){

		dragged=true;
	    if (!isPressed){
	    	dragged=false;
	        return;
	    }

	    float newAngle = angleFromPoint(evt.getNewX(), evt.getNewY());
	    float diff = normalize(newAngle - lastAngle);

	    float dx = evt.getNewX() - centerX;
	    float dy = evt.getNewY() - centerY;
	    float radius = sqrtf(dx*dx + dy*dy);

	    if (radius < 60)
	    {
	        lastAngle = newAngle;
	        return;
	    }

	    accumulatedAngle += diff;

	    float threshold = 0.3f;

	    while (accumulatedAngle > threshold)
	    {
	        increaseVolume();
	        accumulatedAngle -= threshold;
	    }

	    while (accumulatedAngle < -threshold)
	    {
	        decreaseVolume();
	        accumulatedAngle += threshold;
	    }

	    lastAngle = newAngle;
	    dragged=false;
}

void VolumeScreenView::increaseVolume()
{
    changeVolume(1);
}

void VolumeScreenView::decreaseVolume()
{
    changeVolume(-1);
}

void VolumeScreenView::changeVolume(int change)
{
    const int newValue = presenter->getVolume() + change;
    int minValue, maxValue;
    circleProgress.getRange(minValue, maxValue);

    if (newValue >= minValue && newValue <= maxValue)
    {
        presenter->setVolume(newValue);

        // Update the number in the big text field
        bigNumberCustomContainer1.updateValue(newValue);

        if (!isAnimating)
        {
            circleProgress.updateValue(newValue, 0);
            if(newValue == 30 || newValue == 0){
                 haptic.trigger(100);
            }
        }
    }
}

void VolumeScreenView::VolumeScreenLedOn(){
	BSP_LED_SetColor(RGB_BLUE);

}
