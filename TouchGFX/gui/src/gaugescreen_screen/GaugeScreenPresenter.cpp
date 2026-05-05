#include <gui/gaugescreen_screen/GaugeScreenView.hpp>
#include <gui/gaugescreen_screen/GaugeScreenPresenter.hpp>

GaugeScreenPresenter::GaugeScreenPresenter(GaugeScreenView& v)
    : view(v)
{

}

void GaugeScreenPresenter::activate()
{

}

void GaugeScreenPresenter::deactivate()
{

}

void GaugeScreenPresenter::setPressure(int value)
{
    model->setPressure(value);
}

int GaugeScreenPresenter::getPressure()
{
    return model->getPressure();
}

void GaugeScreenPresenter::setAmbientLightRGB(bool red, bool green, bool blue)
{
    model->setAmbientLightRGB(red, green, blue);
}
