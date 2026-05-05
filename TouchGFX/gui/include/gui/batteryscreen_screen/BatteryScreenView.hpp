#ifndef BATTERYSCREENVIEW_HPP
#define BATTERYSCREENVIEW_HPP

#include <gui_generated/batteryscreen_screen/BatteryScreenViewBase.hpp>
#include <gui/batteryscreen_screen/BatteryScreenPresenter.hpp>

class BatteryScreenView : public BatteryScreenViewBase
{
public:
    BatteryScreenView();
    virtual ~BatteryScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();
    virtual void ChargeMenuLedOn() override;
    void BatteryScreenClickHandler (const Box& batteryImage, const ClickEvent& evt);

protected:
    void updateBatteryIndicator();

    int16_t oldBatteryFillLevel = 0x7FFF;
    uint16_t oldBatteryPercentage = 0xFFFF;

    Callback<BatteryScreenView, const Box&, const ClickEvent&> batteryImageCallback;

private:
    uint8_t pressTicks;
    bool isPressed;
    bool longPressTrigerred;
};

#endif // BATTERYSCREENVIEW_HPP
