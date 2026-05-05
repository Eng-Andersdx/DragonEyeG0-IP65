#ifndef VOLUMESCREENVIEW_HPP
#define VOLUMESCREENVIEW_HPP

#include <gui_generated/volumescreen_screen/VolumeScreenViewBase.hpp>
#include <gui/volumescreen_screen/VolumeScreenPresenter.hpp>

//#include <gui/common/PressLogic.hpp>

class VolumeScreenView : public VolumeScreenViewBase
{
public:
    VolumeScreenView();
    virtual ~VolumeScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();

    virtual void increaseVolume();
    virtual void decreaseVolume();
    virtual void VolumeScreenLedOn() override;
    void VolumeScreenClickHandler(const CircleProgress& volumeProgress, const ClickEvent& evt);
    virtual void handleDragEvent(const DragEvent& evt);
    HapticLogic haptic;

protected:

    void changeVolume(int change);

    bool isAnimating = false;

    Callback<VolumeScreenView, const CircleProgress&, const ClickEvent&> volumeImageProgressCallback;

private:
    uint8_t pressTicks;
    bool isPressed;
    bool longPressTrigerred;
    bool dragged;

    float lastAngle;
    float accumulatedAngle;
    bool isTracking;
    float centerX;
    float centerY;

     float angleFromPoint(int16_t x, int16_t y) const;
     float normalize(float a) const;         // [-pi..pi]
};

#endif // VOLUMESCREENVIEW_HPP
