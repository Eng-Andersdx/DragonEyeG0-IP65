#ifndef DELITESPLASHSCREENVIEW_HPP
#define DELITESPLASHSCREENVIEW_HPP

#include <gui_generated/delitesplashscreen_screen/DELiteSplashScreenViewBase.hpp>
#include <gui/delitesplashscreen_screen/DELiteSplashScreenPresenter.hpp>

class DELiteSplashScreenView : public DELiteSplashScreenViewBase
{
public:
    DELiteSplashScreenView();
    virtual ~DELiteSplashScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void WhiteLedOn() override;
    virtual void backlightIntensityControl() override;
protected:
};

#endif // DELITESPLASHSCREENVIEW_HPP
