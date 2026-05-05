#include <gui/gaugescreen_screen/GaugeScreenView.hpp>
#include "DE_Lite/dragoneye_leds.h"
#include "math.h"

extern TIM_HandleTypeDef htim1;

using namespace touchgfx;

// Use TouchGFX' own PI to avoid ambiguity
static const float TWO_PI       = 2.0f * touchgfx::PI;

static const float SELECT_ANGLE = -touchgfx::PI / 2.0f;


GaugeScreenView::GaugeScreenView()
	: guageImageProgressCallback(this, &GaugeScreenView::GaugeScreenClickHandler)
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

void GaugeScreenView::setupScreen()
{
    GaugeScreenViewBase::setupScreen();

    backgroundImage.setClickAction(guageImageProgressCallback);

    presenter->setAmbientLightRGB(true, false, true);

    bigNumberCustomContainer1.updateValue(presenter->getPressure());

    //Setup the intro animation
    isAnimating = true;
    moveNeedleToPostion(0);
    bigNumberCustomContainer1.setVisible(false);
}

void GaugeScreenView::tearDownScreen()
{
    GaugeScreenViewBase::tearDownScreen();
}

void GaugeScreenView::handleTickEvent()
{
    GaugeScreenViewBase::handleTickEvent();

    tickCount++;

    haptic.update();

    if (isPressed && !longPressTrigerred && !dragged){
          	pressTicks++;
             if (pressTicks > 60){
          	   longPressTrigerred= true;
          	   isPressed=false;
          	   application().gotoMenuScreenScreenNoTransition();
          	   pressTicks=0;
             }
           }

    // Set the big number text area visible on the first tick
    if (!bigNumberCustomContainer1.isVisible())
    {
        bigNumberCustomContainer1.setVisible(true);
        bigNumberCustomContainer1.invalidate();
    }

    // Create the intro animation by moving the needle image one position forward until it reaches the actual value
    else if (isAnimating && tickCount % 2 == 1)
    {
        uint16_t currentNeedlePostion = needleImage.getBitmapId() - BITMAP_GAUGE_NEEDLE_00_ID;
        if (currentNeedlePostion < presenter->getPressure())
        {
            moveNeedleToPostion(currentNeedlePostion + 1);
        }
        else
        {
            isAnimating = false;
        }
    }

}

void GaugeScreenView::GaugeScreenClickHandler(const Image& gaugeImage, const ClickEvent& evt){
	if(&gaugeImage == &backgroundImage){
		if (evt.getType() == touchgfx::ClickEvent::PRESSED)
		{
			lastAngle = angleFromPoint(evt.getX(), evt.getY());
			pressTicks=0;
		    isPressed = true;
			longPressTrigerred = false;
		}
		else if (evt.getType() == touchgfx::ClickEvent::RELEASED)
		{
		    isPressed = false;
		}
	}
}


float GaugeScreenView::angleFromPoint(int16_t x, int16_t y) const
{
    float dx = float(x) - centerX;
    float dy = float(y) - centerY;
    return atan2f(dy, dx);   // [-pi..pi], 0 at right
}


float GaugeScreenView::normalize(float a) const
{
    while (a >  touchgfx::PI)  a -= TWO_PI;
    while (a < -touchgfx::PI)  a += TWO_PI;
    return a;
}


void GaugeScreenView::handleDragEvent(const DragEvent& evt){

		dragged=true;
	    if (!isPressed)
	        return;

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
	        increasePressure();
	        accumulatedAngle -= threshold;
	    }

	    while (accumulatedAngle < -threshold)
	    {
	        decreasePressure();
	        accumulatedAngle += threshold;
	    }

	    lastAngle = newAngle;
	    dragged=false;
}

void GaugeScreenView::increasePressure()
{
    changeValue(1);
}

void GaugeScreenView::decreasePressure()
{
    changeValue(-1);
}

void GaugeScreenView::changeValue(const int16_t change)
{
    // Calculate the new value and check if it is in the valid range
    const int16_t newValue = presenter->getPressure() + change;
    if (newValue >= 0 && newValue < NUMBER_OF_POSITIONS)
    {
        presenter->setPressure(newValue);

        // Update the text field
        bigNumberCustomContainer1.updateValue(newValue);

        if (!isAnimating)
        {
            moveNeedleToPostion(newValue);
            if(newValue == 12 || newValue == 0){
                 haptic.trigger(100);
            }
        }
    }
}

void GaugeScreenView::moveNeedleToPostion(uint16_t position)
{
    if (position < NUMBER_OF_POSITIONS)
    {
        // The gauge needle is implemented as a simple image widget
        needleImage.invalidate();
        needleImage.setX(needlePositions[position].x);
        needleImage.setY(needlePositions[position].y);
        needleImage.setBitmap(needlePositions[position].bitmapId);
        needleImage.invalidate();
    }
}

void GaugeScreenView::GuageMenuLedOn(){
	BSP_LED_SetColor(RGB_PURPLE);
}
