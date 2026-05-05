#include <gui/delitesplashscreen_screen/DELiteSplashScreenView.hpp>
#include "DE_Lite/dragoneye_leds.h"

extern TIM_HandleTypeDef htim1;

DELiteSplashScreenView::DELiteSplashScreenView()
{

}

void DELiteSplashScreenView::setupScreen()
{
    DELiteSplashScreenViewBase::setupScreen();
}

void DELiteSplashScreenView::tearDownScreen()
{
    DELiteSplashScreenViewBase::tearDownScreen();
}

void DELiteSplashScreenView::WhiteLedOn(){
	BSP_LED_PWM_Init(&htim1, TIM_CHANNEL_4, TIM_CHANNEL_3, TIM_CHANNEL_2);
	BSP_LED_SetColor(RGB_WHITE);
}

void DELiteSplashScreenView::backlightIntensityControl(){
	BSP_BL_PWM_Init(&htim1, TIM_CHANNEL_1);
	BSP_BL_SetIntensity(100);
}
