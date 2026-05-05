#ifndef MENUITEMS_HPP
#define MENUITEMS_HPP

#include <images/BitmapDatabase.hpp>
#include <texts/TextKeysAndLanguages.hpp>

struct MenuItem
{
    uint16_t iconId;
    uint16_t gradientId;
    uint16_t titleId;
};

static const MenuItem menuItems[] =
{
    {
        BITMAP_HEAT_MENU_ICON_ID,
        BITMAP_HEAT_MENU_GRADIENT_ID,
        T_MENU_TEMPERATURE
    },
    {
        BITMAP_BATTERY_MENU_ICON_ID,
        BITMAP_BATTERY_MENU_GRADIENT_ID,
        T_MENU_BATTERY
    },
    {
        BITMAP_SOUND_MENU_ICON_ID,
        BITMAP_SOUND_MENU_GRADIENT_ID,
        T_MENU_VOLUME
    },
    {
        BITMAP_GAUGE_MENU_ICON_ID,
        BITMAP_GAUGE_MENU_GRADIENT_ID,
        T_MENU_GUAGE
    },
    {
        BITMAP_LIGHT_MENU_ICON_ID,
        BITMAP_LIGHT_MENU_GRADIENT_ID,
        T_MENU_LIGHT
    },
    {
        BITMAP_INFO_MENU_ICON_ID,
        BITMAP_INFO_MENU_GRADIENT_ID,
        T_MENU_INFO
    }
};

#endif
