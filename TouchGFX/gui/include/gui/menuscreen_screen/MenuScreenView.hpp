#ifndef MENUSCREENVIEW_HPP
#define MENUSCREENVIEW_HPP

#include <gui_generated/menuscreen_screen/MenuScreenViewBase.hpp>
#include <gui/menuscreen_screen/MenuScreenPresenter.hpp>
#include <touchgfx/widgets/TextureMapper.hpp>
#include <gui/common/HapticLogic.hpp>

class MenuScreenView : public MenuScreenViewBase
{
public:
    MenuScreenView();
    virtual ~MenuScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void menuScrollWheelUpdateItem(MenuIconCustomContainer& item, int16_t itemIndex);

    virtual void goRight();
    virtual void goLeft();
    virtual void enter();
    virtual void handleTickEvent();
    virtual void MainMenuLedOn() override;
    virtual void handleDragEvent(const DragEvent& evt);
    virtual void handleClickEvent(const ClickEvent& event);

    // Callbacks into the view
    void setActivatedCallback(touchgfx::GenericCallback<int>& cb);


    enum {
    	DEMO_HEAT,
		DEMO_BATTERY,
		DEMO_SOUND,
		DEMO_GAUGE,
		DEMO_LIGHT
    };

protected:
    void rollMenu(int16_t change);

private:

    // Geometry
    float centerX;
    float centerY;
	float startX;
	float startY;


    HapticLogic haptic;

    // Animation / drag state
    bool longPressTrigerred;

    float accumulatedDy;

};

#endif // MENUSCREENVIEW_HPP
