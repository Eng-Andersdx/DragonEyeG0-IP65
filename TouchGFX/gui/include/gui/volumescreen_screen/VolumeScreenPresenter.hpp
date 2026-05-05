#ifndef VOLUMESCREENPRESENTER_HPP
#define VOLUMESCREENPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class VolumeScreenView;

class VolumeScreenPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    VolumeScreenPresenter(VolumeScreenView& v);

    /**
     * The activate function is called automatically when this screen is "switched in"
     * (ie. made active). Initialization logic can be placed here.
     */
    virtual void activate();

    /**
     * The deactivate function is called automatically when this screen is "switched out"
     * (ie. made inactive). Teardown functionality can be placed here.
     */
    virtual void deactivate();

    virtual ~VolumeScreenPresenter() {}

    void setVolume(int value);
    int getVolume();

    void setAmbientLightRGB(bool red, bool green, bool blue);

private:
    VolumeScreenPresenter();

    VolumeScreenView& view;
};

#endif // VOLUMESCREENPRESENTER_HPP
