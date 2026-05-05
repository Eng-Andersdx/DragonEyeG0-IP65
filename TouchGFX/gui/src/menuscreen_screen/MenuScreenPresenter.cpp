#include <gui/menuscreen_screen/MenuScreenView.hpp>
#include <gui/menuscreen_screen/MenuScreenPresenter.hpp>

MenuScreenPresenter::MenuScreenPresenter(MenuScreenView& v)
    : view(v)
{

}

void MenuScreenPresenter::activate()
{

}

void MenuScreenPresenter::deactivate()
{

}

void MenuScreenPresenter::setSelectedDemoNumber(int value)
{
    model->setSelectedDemoNumber(value);
}

int MenuScreenPresenter::getSelectedDemoNumber()
{
    return model->getSelectedDemoNumber();
}

void MenuScreenPresenter::setAmbientLightRGB(bool red, bool green, bool blue)
{
    model->setAmbientLightRGB(red, green, blue);
}
