#include <gui/volumescreen_screen/VolumeScreenView.hpp>
#include <gui/volumescreen_screen/VolumeScreenPresenter.hpp>

VolumeScreenPresenter::VolumeScreenPresenter(VolumeScreenView& v)
    : view(v)
{

}

void VolumeScreenPresenter::activate()
{

}

void VolumeScreenPresenter::deactivate()
{

}

void VolumeScreenPresenter::setVolume(int value)
{
    model->setVolume(value);
}

int VolumeScreenPresenter::getVolume()
{
    return model->getVolume();
}

void VolumeScreenPresenter::setAmbientLightRGB(bool red, bool green, bool blue)
{
    model->setAmbientLightRGB(red, green, blue);
}
