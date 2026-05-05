#ifndef DELITESPLASHSCREENPRESENTER_HPP
#define DELITESPLASHSCREENPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class DELiteSplashScreenView;

class DELiteSplashScreenPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    DELiteSplashScreenPresenter(DELiteSplashScreenView& v);

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

    virtual ~DELiteSplashScreenPresenter() {}

private:
    DELiteSplashScreenPresenter();

    DELiteSplashScreenView& view;
};

#endif // DELITESPLASHSCREENPRESENTER_HPP
