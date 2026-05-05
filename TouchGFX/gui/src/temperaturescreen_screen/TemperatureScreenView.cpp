#include <gui/temperaturescreen_screen/TemperatureScreenView.hpp>
#include <touchgfx/Color.hpp>
#include "DE_Lite/dragoneye_leds.h"
#include "math.h"

extern TIM_HandleTypeDef htim1;

TemperatureScreenView::TemperatureScreenView()
	:temperatureImageProgressCallback(this, &TemperatureScreenView::TemperatureScreenClickHandler)
	, pressTicks(0)
	, isPressed(false)
	, longPressTrigerred(false)
	, dragged(false)
	, accumulatedDy(0)
{

}

void TemperatureScreenView::setupScreen()
{
    TemperatureScreenViewBase::setupScreen();

    image2.setClickAction(temperatureImageProgressCallback);

    // Update the number in the big text field(s)
    int16_t currentTemperautre = presenter->getTemperature();
    bigNumberCustomContainer1.updateValue(currentTemperautre);
    bigNumberCustomContainer1.updateColor(touchgfx::Color::getColorFromRGB(0, 0, 0));
    bigNumberCustomContainer2.updateValue(currentTemperautre);
    bigNumberCustomContainer2.updateColor(touchgfx::Color::getColorFromRGB(229, 36, 62));

    presenter->setAmbientLightRGB(true, false, false);

    //Setup the intro animation
    fillContainer.setHeight(HAL::DISPLAY_HEIGHT);
    isAnimating = true;

    // Hide the big number text field to shorten the initial screen rendertime
    bigNumberCustomContainer2.setVisible(false);
}

void TemperatureScreenView::tearDownScreen()
{
    TemperatureScreenViewBase::tearDownScreen();
}

void TemperatureScreenView::handleTickEvent()
{
    TemperatureScreenViewBase::handleTickEvent();

    haptic.update();

    if (isPressed && !longPressTrigerred && !dragged){
       	pressTicks++;
          if (pressTicks > 60){
       	   longPressTrigerred = true;
       	   isPressed=false;
       	   application().gotoMenuScreenScreenNoTransition();
       	   pressTicks=0;
          }
    }

    // Set the number text field visible on the first tick
    if (!bigNumberCustomContainer2.isVisible())
    {
        bigNumberCustomContainer2.setVisible(true);
        bigNumberCustomContainer2.invalidate();
    }

    // Create the intro animation by gradually increasing the fill area until the actual level is reached
    else if (isAnimating)
    {
        if (fillContainer.getHeight() > HAL::DISPLAY_HEIGHT - (presenter->getTemperature() - MIN_VALUE) * LINES_PER_DEGREE)
        {
            updateFill(fillContainer.getHeight() - 8);
        }
        else
        {
            isAnimating = false;
        }
    }
}

void TemperatureScreenView::TemperatureScreenClickHandler(const Image& img, const ClickEvent& evt){
	if(&img == &image2 && !dragged){
		if (evt.getType() == touchgfx::ClickEvent::PRESSED)
		    {
				pressTicks =0;
		        isPressed = true;
		        longPressTrigerred= false;
		    }
		    else if (evt.getType() == touchgfx::ClickEvent::RELEASED)
		    {
		        isPressed = false;
		    }
	}
}

void TemperatureScreenView::handleDragEvent(const DragEvent& evt){

		dragged =true;
		if (!isPressed)
			return;
		accumulatedDy+=evt.getDeltaY();
		const float stepSize = 30.0f;
		const float triggerPoint = 20.0f;
		while (accumulatedDy > triggerPoint){
			decreaseTemperature();
			accumulatedDy -= stepSize;
		}
		while (accumulatedDy < -triggerPoint)
		{
			increaseTemperature();
		    accumulatedDy += stepSize;
		}
		dragged=false;
}

void TemperatureScreenView::increaseTemperature()
{
    changeTemperature(1);
}

void TemperatureScreenView::decreaseTemperature()
{
    changeTemperature(-1);
}

void TemperatureScreenView::changeTemperature(const int16_t change)
{
    int16_t newTemperature = presenter->getTemperature() + change;
    if (newTemperature >= MIN_VALUE && newTemperature <= MAX_VALUE)
    {
        presenter->setTemperature(newTemperature);

        // Update the value in the big text field
        bigNumberCustomContainer1.updateValue(newTemperature);
        bigNumberCustomContainer2.updateValue(newTemperature);

        if (!isAnimating)
        {
            const int16_t newLevel = HAL::DISPLAY_HEIGHT - (newTemperature - MIN_VALUE) * LINES_PER_DEGREE;
            updateFill(newLevel);
            if(newTemperature == MAX_VALUE || newTemperature == MIN_VALUE){
                 haptic.trigger(100);
            }
        }
    }
}

void TemperatureScreenView::updateFill(const int16_t newLevel)
{
    // Change the container height, to change the fill level
    const int16_t oldLevel = fillContainer.getHeight();
    fillContainer.setHeight(newLevel);

    // Invalidate the screen area which was changed
    Rect updatedFill;
    updatedFill.x = 0;
    updatedFill.width = 240;
    if (newLevel > oldLevel)
    {
        updatedFill.y = oldLevel;
        updatedFill.height = newLevel - oldLevel;
    }
    else
    {
        updatedFill.y = newLevel;
        updatedFill.height = oldLevel - newLevel;
    }
    invalidateRect(updatedFill);
}

void TemperatureScreenView::TemperatureMenuLedOn(){
	BSP_LED_SetColor(RGB_RED);
}
