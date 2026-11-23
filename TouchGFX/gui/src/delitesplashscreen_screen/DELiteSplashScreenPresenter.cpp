#include <gui/delitesplashscreen_screen/DELiteSplashScreenView.hpp>
#include <gui/delitesplashscreen_screen/DELiteSplashScreenPresenter.hpp>

DELiteSplashScreenPresenter::DELiteSplashScreenPresenter(DELiteSplashScreenView& v)
    : view(v)
{

}

void DELiteSplashScreenPresenter::activate()
{

}

void DELiteSplashScreenPresenter::deactivate()
{

}

void DELiteSplashScreenPresenter::setAmbientLightRGB(uint8_t red, uint8_t green, uint8_t blue)
{
    model->setAmbientLightRGB(red, green, blue);
}
