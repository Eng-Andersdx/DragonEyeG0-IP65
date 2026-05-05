#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>
#include <touchgfx/Utils.hpp>
#include <gui/common/MenuItems.hpp>

#ifndef SIMULATOR
#include "main.h"
#endif

Model::Model() :
    modelListener(0)
{
}

void Model::tick()
{
    tickCount++;

    // auto-increment the battery level (only for demo animation purpose)
    if (batteryLevel < 1000)
    {
        batteryLevel += 2;
    }
}

void Model::setAmbientLightRGB(bool red, bool green, bool blue)
{
    touchgfx_printf("Ambient light set to: Red = %i, Green = %i, Blue = %i\r\n", red, green, blue); // Print to TouchGFX simulator debug log

#ifdef LED_RED_GPIO_Port
    HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, red ? GPIO_PIN_SET : GPIO_PIN_RESET);
#endif
#ifdef LED_BLUE_GPIO_Port
    HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, blue ? GPIO_PIN_SET : GPIO_PIN_RESET);
#endif
#ifdef LED_GREEN_GPIO_Port
    HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, green ? GPIO_PIN_SET : GPIO_PIN_RESET);
#endif
}

void Model::sendCommand(char type, char value)
{
    uint8_t command[] = "X:000\r\n";
    command[0] = type;
    command[2] = '0' + value / 100;
    command[3] = '0' + (value % 100) / 10;
    command[4] = '0' + value % 10;

    // Print to TouchGFX simulator debug log
    touchgfx_printf("Transmitted on UART: "); 
    touchgfx_printf((char*)command);

#ifdef HAL_UART_MODULE_ENABLED
//    extern UART_HandleTypeDef huart1;
//    HAL_UART_Transmit(&huart1, command, sizeof(command), 100);
#endif
}

void Model::setSelectedDemoNumber(int value)
{
    selectedDemoNumber = value;

    // reset battery level when battery screen is entered (only for demo animation purpose)
    if (menuItems[value].titleId == T_MENU_BATTERY)
    {
        batteryLevel = 0;
    }
}

int Model::getSelectedDemoNumber()
{
    return selectedDemoNumber;
}

void Model::setVolume(int value)
{
    volume = value;
    sendCommand('V', volume);
}

int Model::getVolume()
{
    return volume;
}

void Model::setPressure(int value)
{
    pressure = value;
    sendCommand('P', pressure);
}

int Model::getPressure()
{
    return pressure;
}

void Model::setBrightness(int value)
{
    brightness = value;
    sendCommand('B', brightness);
}

int Model::getBrightness()
{
    return brightness;
}

void Model::setTemperature(int value)
{
    temperature = value;
    sendCommand('T', temperature);
}

int Model::getTemperature()
{
    return temperature;
}

int Model::getBatteryLevel()
{
    return batteryLevel;
}
