#include <gui/MenuScreen_screen/MenuScreenView.hpp>
#include <gui/common/MenuItems.hpp>
#include <gui/common/LedColors.hpp>

MenuScreenView::MenuScreenView()
    : lastItem(-1)
{
}

void MenuScreenView::setMenuLed(int index)
{
    switch (menuItems[index].titleId)
    {
    case T_MENU_TEMPERATURE: presenter->setAmbientLightRGB(LED_RED);    break;
    case T_MENU_BATTERY:     presenter->setAmbientLightRGB(LED_GREEN);  break;
    case T_MENU_VOLUME:      presenter->setAmbientLightRGB(LED_BLUE);   break;
    case T_MENU_GUAGE:       presenter->setAmbientLightRGB(LED_PURPLE); break;
    case T_MENU_LIGHT:       presenter->setAmbientLightRGB(LED_YELLOW); break;
    default:                 presenter->setAmbientLightRGB(LED_CYAN);   break;   // Info
    }
}

void MenuScreenView::setupScreen()
{
    MenuScreenViewBase::setupScreen();

    const int sel = presenter->getSelectedDemoNumber();
    menuScrollWheel.animateToItem(sel, 0);
    lastItem = sel;
    setMenuLed(sel);                       // colour on entry, no haptic

    titleTextArea.setVisible(false);
    gradientImage.setVisible(false);
}

void MenuScreenView::tearDownScreen()
{
    MenuScreenViewBase::tearDownScreen();
}

void MenuScreenView::menuScrollWheelUpdateItem(MenuIconCustomContainer& item, int16_t itemIndex)
{
    // Update the menu elements based on their indicies
    item.setIndex(itemIndex);
}

void MenuScreenView::handleTickEvent()
{
    MenuScreenViewBase::handleTickEvent();

    haptic.update();                       // switches the haptic off after its time

    const int16_t cur = menuScrollWheel.getSelectedItem();
    if (cur != lastItem && cur >= 0 && cur < menuScrollWheel.getNumberOfItems())
    {
        lastItem = cur;
        setMenuLed(cur);
        haptic.trigger(100);
    }

    if (menuScrollWheel.isAnimating() && titleTextArea.isVisible())
    {
        titleTextArea.setVisible(false);
        titleTextArea.invalidate();
        gradientImage.setVisible(false);
        gradientImage.invalidate();
    }

    // Detect when menu wheel stops animating to an icon
    if (!menuScrollWheel.isAnimating() && !titleTextArea.isVisible())
    {
        MenuItem activeItem = menuItems[menuScrollWheel.getSelectedItem()];

        // Start title text animation
        titleTextArea.setTypedText(touchgfx::TypedText(activeItem.titleId));
        titleTextArea.setAlpha(0);
        titleTextArea.startFadeAnimation(255, 10, EasingEquations::quadEaseIn);
        titleTextArea.setVisible(true);

        // Start color gradient animation
        gradientImage.setBitmap(activeItem.gradientId);
        gradientImage.setX(240);
        gradientImage.startMoveAnimation(240 - gradientImage.getWidth(), 0, 10, EasingEquations::quadEaseIn);
        gradientImage.setVisible(true);
    }
}

//void MenuScreenView::handleClickEvent(const ClickEvent& event)
//{
//    /* 2026-11-13: tap-enter now comes from the touch driver as key 53 (Interaction "enter").
//     * Entering here too would enter twice and bounce back. Previous code kept below. */
////    MenuScreenViewBase::handleClickEvent(event);
////    return;
//
////    if (event.getType() == touchgfx::ClickEvent::ClickEventType::RELEASED
////    		&&
////            !menuScrollWheel.isAnimating() &&
////            event.getY() >= 20 &&
////            event.getY() < 224)
////    {
////        // Enter sub demo, when the menu is standing still and the selected icon is clicked
////        enter();
////    }
////    else
////    {
//        // Otherwise, propagate the click event to the default handling
//        MenuScreenViewBase::handleClickEvent(event);
////    }
//}

void MenuScreenView::rollMenu(int16_t change)
{
    // Animate the menu wheel to a new position, if it is valid
    int newIndex = menuScrollWheel.getSelectedItem() + change;
    if (newIndex >= 0 && newIndex < menuScrollWheel.getNumberOfItems())
    {
        menuScrollWheel.animateToItem(newIndex, 20);

        // Hide the title and color gradient during the menu animation
        titleTextArea.setVisible(false);
        titleTextArea.invalidate();
        gradientImage.setVisible(false);
        gradientImage.invalidate();
    }
}

void MenuScreenView::scrollDown()
{
    rollMenu(1);
}

void MenuScreenView::scrollUp()
{
    rollMenu(-1);
}

void MenuScreenView::enter()
{
    // Update the parameter in the model, to be able to start at the same position when returning to the menu
    presenter->setSelectedDemoNumber(menuScrollWheel.getSelectedItem());

    // Use the text for the selected icon to determine which screen to switch to
    switch (menuItems[menuScrollWheel.getSelectedItem()].titleId)
    {
    case T_MENU_TEMPERATURE:
        goToTemperature();
        break;
    case T_MENU_BATTERY:
        goToBattery();
        break;
    case T_MENU_VOLUME:
        goToVolume();
        break;
    case T_MENU_GUAGE:
        goToGauge();
        break;
    case T_MENU_LIGHT:
        goToLight();
        break;
    case T_MENU_INFO:
        goToInfo();
        break;
    default:
        break;
    }
}
