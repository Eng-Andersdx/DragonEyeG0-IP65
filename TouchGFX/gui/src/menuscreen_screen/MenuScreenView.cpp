#include <gui/MenuScreen_screen/MenuScreenView.hpp>
#include <gui/common/MenuItems.hpp>
#include "DE_Lite/dragoneye_leds.h"
#include "DE_Lite/de_lite_haptic_sensor.h"
#include "math.h"

extern TIM_HandleTypeDef htim1;

using namespace touchgfx;


MenuScreenView::MenuScreenView()
	: longPressTrigerred(false)
	, accumulatedDy(0)
	, startX(0.f)
	, startY(0.f)


{
}

void MenuScreenView::setupScreen()
{
    MenuScreenViewBase::setupScreen();

    switch(presenter->getSelectedDemoNumber()){
        	case DEMO_HEAT:
        		BSP_LED_SetColor(RGB_RED);
        		break;
        	case DEMO_BATTERY:
        		BSP_LED_SetColor(RGB_GREEN);
        		break;
        	case DEMO_SOUND:
        		BSP_LED_SetColor(RGB_BLUE);
        		break;
        	case DEMO_GAUGE:
        		BSP_LED_SetColor(RGB_PURPLE);
        		break;
        	case DEMO_LIGHT:
        		BSP_LED_SetColor(RGB_YELLOW);
        		break;
        	default:
        		BSP_LED_SetColor(RGB_CYAN);
        		break;
    }

    menuScrollWheel.animateToItem(presenter->getSelectedDemoNumber(), 0);

    // Hide the widgets that will animate in when the wheel stops
    titleTextArea.setVisible(false);
    gradientImage.setVisible(false);

    presenter->setAmbientLightRGB(true, true, true);
}

void MenuScreenView::tearDownScreen()
{
    MenuScreenViewBase::tearDownScreen();
}

void MenuScreenView::menuScrollWheelUpdateItem(MenuIconCustomContainer& item, int16_t itemIndex)
{
    // Update the menu elements based on their indicies
	item.setIndex(itemIndex);

}

void MenuScreenView::handleTickEvent()
{
    MenuScreenViewBase::handleTickEvent();

    haptic.update();

    // Detect when menu wheel stops animating to an icon
    if (!menuScrollWheel.isAnimating() && !titleTextArea.isVisible())
    {
        MenuItem activeItem = menuItems[menuScrollWheel.getSelectedItem()];

        // Start title text animation
        titleTextArea.setTypedText(touchgfx::TypedText(activeItem.titleId));
        titleTextArea.setAlpha(0);
        titleTextArea.startFadeAnimation(255, 10, EasingEquations::quadEaseIn);
        titleTextArea.setVisible(true);

        // Start color gradient animation
        gradientImage.setBitmap(activeItem.gradientId);
        gradientImage.setX(240);
        gradientImage.startMoveAnimation(240 - gradientImage.getWidth(), 0, 10, EasingEquations::quadEaseIn);
        gradientImage.setVisible(true);
    }
}

void MenuScreenView::handleClickEvent(const ClickEvent& evt){
	if(evt.getType() == ClickEvent::PRESSED){
		startX=evt.getX();
		startY=evt.getY();
	}
	else if (evt.getType() == ClickEvent::RELEASED){
		float dxTotal=evt.getX()-startX;
		float dyTotal=evt.getY()-startY;
		float movement= sqrtf(dxTotal*dxTotal+dyTotal*dyTotal);
		float dx=evt.getX()-55;
		float dy=evt.getY()-120;
		float radius = sqrtf(dx*dx + dy*dy);
		if(movement < 10.f && radius <=50){
			enter();
		}
	}
}



void MenuScreenView::handleDragEvent(const DragEvent& evt){
		float dy = evt.getDeltaY();
		accumulatedDy+=dy;
		const float stepSize = 120.0f;
		const float triggerPoint = 60.0f;

		while (accumulatedDy > triggerPoint){
			goLeft();
			accumulatedDy -= stepSize;
		}
		while (accumulatedDy < -triggerPoint)
		{
			goRight();
		    accumulatedDy += stepSize;
		}

}

void MenuScreenView::rollMenu(int16_t change)
{
    // Animate the menu wheel to a new position, if it is valid
    int newIndex = menuScrollWheel.getSelectedItem() + change;
    if (newIndex >= 0 && newIndex < menuScrollWheel.getNumberOfItems()){
    	switch(menuItems[newIndex].iconId){
    	case BITMAP_HEAT_MENU_ICON_ID:
    		BSP_LED_SetColor(RGB_RED);
    		haptic.trigger(100);
    		break;
    	case BITMAP_BATTERY_MENU_ICON_ID:
    		BSP_LED_SetColor(RGB_GREEN);
    		haptic.trigger(100);
    		break;
    	case BITMAP_SOUND_MENU_ICON_ID:
    		BSP_LED_SetColor(RGB_BLUE);
    		haptic.trigger(100);
    		break;
    	case BITMAP_GAUGE_MENU_ICON_ID:
    		BSP_LED_SetColor(RGB_PURPLE);
    		haptic.trigger(100);
    		break;
    	case BITMAP_LIGHT_MENU_ICON_ID:
    		BSP_LED_SetColor(RGB_YELLOW);
    		haptic.trigger(100);
    		break;
    	default:
    		BSP_LED_SetColor(RGB_CYAN);
    		haptic.trigger(100);
    		break;
    	}

        menuScrollWheel.animateToItem(newIndex, 20);

        // Hide the title and color gradient during the menu animation
        titleTextArea.setVisible(false);
        titleTextArea.invalidate();
        gradientImage.setVisible(false);
        gradientImage.invalidate();
    }
}

void MenuScreenView::goRight()
{
    rollMenu(1);
}

void MenuScreenView::goLeft()
{
   rollMenu(-1);
}

void MenuScreenView::enter()
{
    // Update the parameter in the model, to be able to start at the same position when returning to the menu
    presenter->setSelectedDemoNumber(menuScrollWheel.getSelectedItem());
    // Use the text for the selected icon to determine which screen to switch to
    switch (menuItems[menuScrollWheel.getSelectedItem()].titleId)
    {
    case T_MENU_TEMPERATURE:
        goToTemperature();
        break;
    case T_MENU_BATTERY:
        goToBattery();
        break;
    case T_MENU_VOLUME:
        goToVolume();
        break;
    case T_MENU_GUAGE:
        goToSpeed();
        break;
    case T_MENU_LIGHT:
        goToLight();
        break;
    case T_MENU_INFO:
        goToInfo();
        break;
    default:
        break;
    }
}

void MenuScreenView::MainMenuLedOn(){
	BSP_LED_SetColor(RGB_GREEN);
}
