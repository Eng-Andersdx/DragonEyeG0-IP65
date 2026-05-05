#include <gui/temperaturescreen_screen/TemperatureScreenView.hpp>
#include <gui/temperaturescreen_screen/TemperatureScreenPresenter.hpp>

TemperatureScreenPresenter::TemperatureScreenPresenter(TemperatureScreenView& v)
    : view(v)
{

}

void TemperatureScreenPresenter::activate()
{

}

void TemperatureScreenPresenter::deactivate()
{

}

void TemperatureScreenPresenter::setTemperature(int value)
{
    model->setTemperature(value);
}

int TemperatureScreenPresenter::getTemperature()
{
    return model->getTemperature();
}

void TemperatureScreenPresenter::setAmbientLightRGB(bool red, bool green, bool blue)
{
    model->setAmbientLightRGB(red, green, blue);
}
