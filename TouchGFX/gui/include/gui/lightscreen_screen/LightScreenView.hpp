#ifndef LIGHTSCREENVIEW_HPP
#define LIGHTSCREENVIEW_HPP

#include <gui_generated/lightscreen_screen/LightScreenViewBase.hpp>
#include <gui/lightscreen_screen/LightScreenPresenter.hpp>

class LightScreenView : public LightScreenViewBase
{
public:
    LightScreenView();
    virtual ~LightScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();

    virtual void increaseBrightness();
    virtual void decreaseBrightness();
    virtual void LightScreenLedOn() override;
    void LightScreenClickHandler(const ImageProgress& lightImage, const ClickEvent& evt);
    virtual void handleDragEvent(const DragEvent& evt);

    HapticLogic haptic;

protected:
    const uint16_t FULL_CIRCLE = 360;
    const uint16_t NUMBER_OF_STEPS = 20;
    const uint16_t ANIMATION_SPEED = 18;
    const uint16_t MIN_VALUE = 0;
    const uint16_t MAX_VALUE = 100;

    void changeBrightness(int change);

    bool isAnimating = false;
    Callback<LightScreenView, const ImageProgress&, const ClickEvent&> lightImageProgressCallback;

private:
	uint8_t pressTicks;
    bool isPressed;
    bool longPressTrigerred;
    bool dragged;
    float accumulatedDy;
};

#endif // LIGHTSCREENVIEW_HPP
