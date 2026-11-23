#include <gui/infoscreen2_screen/InfoScreen2View.hpp>
#include <gui/infoscreen2_screen/InfoScreen2Presenter.hpp>

InfoScreen2Presenter::InfoScreen2Presenter(InfoScreen2View& v)
    : view(v)
{

}

void InfoScreen2Presenter::activate()
{

}

void InfoScreen2Presenter::deactivate()
{

}

void InfoScreen2Presenter::setAmbientLightRGB(uint8_t red, uint8_t green, uint8_t blue)
{
    model->setAmbientLightRGB(red, green, blue);
}
