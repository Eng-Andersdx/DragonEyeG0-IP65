#ifndef TEMPERATURESCREENVIEW_HPP
#define TEMPERATURESCREENVIEW_HPP

#include <gui_generated/temperaturescreen_screen/TemperatureScreenViewBase.hpp>
#include <gui/temperaturescreen_screen/TemperatureScreenPresenter.hpp>


class TemperatureScreenView : public TemperatureScreenViewBase
{
public:
    TemperatureScreenView();
    virtual ~TemperatureScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();

    virtual void increaseTemperature();
    virtual void decreaseTemperature();
    virtual void TemperatureMenuLedOn() override;
    virtual void handleDragEvent(const DragEvent& evt);
    void TemperatureScreenClickHandler(const Image& image2, const ClickEvent& evt);

    HapticLogic haptic;

protected:
    const uint16_t MIN_VALUE = 5;
    const uint16_t MAX_VALUE = 35;
    const uint16_t LINES_PER_DEGREE = HAL::DISPLAY_HEIGHT / (MAX_VALUE - MIN_VALUE);

    void changeTemperature(const int16_t change);
    void updateFill(const int16_t newLevel);

    bool isAnimating = false;
    Callback<TemperatureScreenView, const Image&, const ClickEvent&> temperatureImageProgressCallback;

private:
    uint8_t pressTicks;
    bool isPressed;
    bool longPressTrigerred;
    bool dragged;
    float accumulatedDy;
};

#endif // TEMPERATURESCREENVIEW_HPP
