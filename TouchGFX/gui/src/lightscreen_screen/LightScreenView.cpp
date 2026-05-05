#include <gui/lightscreen_screen/LightScreenView.hpp>
#include "DE_Lite/dragoneye_leds.h"
#include "math.h"

extern TIM_HandleTypeDef htim1;

LightScreenView::LightScreenView()
	: lightImageProgressCallback(this, &LightScreenView::LightScreenClickHandler)
	, pressTicks(0)
	, isPressed(false)
	, longPressTrigerred(false)
	,dragged(false)
	,accumulatedDy(0)
{

}

void LightScreenView::setupScreen()
{
    LightScreenViewBase::setupScreen();

    lightImageProgress.setClickAction(lightImageProgressCallback);

    presenter->setAmbientLightRGB(true, true, false);

    Unicode::snprintf(lightTextAreaBuffer, LIGHTTEXTAREA_SIZE, "%d", presenter->getBrightness());

    //Setup the intro animation
    lightImageProgress.updateValue(0, 0);
    isAnimating = true;

    // Hide the big text areas to shorten the initial rendertime of the screen
    lightTextArea.setVisible(false);
    percentTextArea.setVisible(false);
}

void LightScreenView::tearDownScreen()
{
    LightScreenViewBase::tearDownScreen();
}

void LightScreenView::handleTickEvent()
{
    LightScreenViewBase::handleTickEvent();
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

    // Set the hidden text areas visible on first tick
    if (!lightTextArea.isVisible())
    {
        lightTextArea.setVisible(true);
        lightTextArea.invalidate();
        percentTextArea.setVisible(true);
        percentTextArea.invalidate();
    }
    else if (isAnimating)
    {
        // Create the animation by increasing the fill size until the actual value is reached
        int minValue, maxValue;
        lightImageProgress.getRange(minValue, maxValue);
        uint16_t currentProgress = lightImageProgress.getProgress(maxValue - minValue);
        if (currentProgress < presenter->getBrightness())
        {
            lightImageProgress.updateValue(currentProgress + 5, 0);
        }
        else
        {
            isAnimating = false;
        }
    }
}

void LightScreenView::handleDragEvent(const DragEvent& evt){

		dragged=true;
		if (!isPressed)
			return;
		accumulatedDy+=evt.getDeltaY();
		const float stepSize = 30.0f;
		const float triggerPoint = 20.0f;
		while (accumulatedDy > triggerPoint){
			decreaseBrightness();
			accumulatedDy -= stepSize;
		}
		while (accumulatedDy < -triggerPoint)
		{
			increaseBrightness();
		    accumulatedDy += stepSize;
		}
		dragged=false;
}

void LightScreenView::LightScreenClickHandler(const ImageProgress& lightImage, const ClickEvent& evt){
	if(&lightImage == &lightImageProgress && !dragged){
		 if (evt.getType() == touchgfx::ClickEvent::PRESSED)
		 {
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

void LightScreenView::increaseBrightness()
{
    changeBrightness(MAX_VALUE / NUMBER_OF_STEPS);
}

void LightScreenView::decreaseBrightness()
{
    changeBrightness(-MAX_VALUE / NUMBER_OF_STEPS);
}

void LightScreenView::changeBrightness(int change)
{
    const int newValue = presenter->getBrightness() + change;
    int minValue, maxValue;
    lightImageProgress.getRange(minValue, maxValue);

    if (newValue >= minValue && newValue <= maxValue)
    {
        presenter->setBrightness(newValue);

        // Update the number text
        Unicode::snprintf(lightTextAreaBuffer, LIGHTTEXTAREA_SIZE, "%d", newValue);
        lightTextArea.invalidate();

        // Only update the graphics after the intro initial animation is done
        if (!isAnimating)
        {
            lightImageProgress.updateValue(newValue, 0);
            if(newValue == MAX_VALUE || newValue == MIN_VALUE){
            	haptic.trigger(100);
            }
        }
    }
}

void LightScreenView::LightScreenLedOn(){
	BSP_LED_SetColor(RGB_YELLOW);
}
