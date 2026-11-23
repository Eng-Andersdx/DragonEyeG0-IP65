#include <gui/delitesplashscreen_screen/DELiteSplashScreenView.hpp>
#include <gui/common/LedColors.hpp>      /* 2026-11-23: LED colours via presenter, no BSP include */

#ifdef KNOB_INTERFACE
#include "knob_interface.hpp"
#endif

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
	presenter->setAmbientLightRGB(LED_WHITE);
}

void DELiteSplashScreenView::backlightIntensityControl(){
#ifdef KNOB_INTERFACE
	knobSetBacklight(100);
#endif
}
