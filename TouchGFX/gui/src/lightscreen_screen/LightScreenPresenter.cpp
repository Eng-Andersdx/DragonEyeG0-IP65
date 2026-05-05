#include <gui/lightscreen_screen/LightScreenView.hpp>
#include <gui/lightscreen_screen/LightScreenPresenter.hpp>

LightScreenPresenter::LightScreenPresenter(LightScreenView& v)
    : view(v)
{

}

void LightScreenPresenter::activate()
{

}

void LightScreenPresenter::deactivate()
{

}

void LightScreenPresenter::setBrightness(int value)
{
    model->setBrightness(value);
}

int LightScreenPresenter::getBrightness()
{
    return model->getBrightness();
}

void LightScreenPresenter::setAmbientLightRGB(bool red, bool green, bool blue)
{
    model->setAmbientLightRGB(red, green, blue);
}
