#ifndef INFOSCREEN2VIEW_HPP
#define INFOSCREEN2VIEW_HPP

#include <gui_generated/infoscreen2_screen/InfoScreen2ViewBase.hpp>
#include <gui/infoscreen2_screen/InfoScreen2Presenter.hpp>


class InfoScreen2View : public InfoScreen2ViewBase
{
public:
    InfoScreen2View();
    virtual ~InfoScreen2View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void infoScreenLedOn();
    virtual void handleTickEvent();
    void InfoScreenClickHandler(const Box& backgroundBox, const ClickEvent& evt);
protected:
    Callback<InfoScreen2View, const Box&, const ClickEvent&> infoScreenCallback;

private:
	uint8_t pressTicks;
    bool isPressed;
    bool longPressTrigerred;
    bool dragged;
    float accumulatedDx;
};

#endif // INFOSCREEN2VIEW_HPP
