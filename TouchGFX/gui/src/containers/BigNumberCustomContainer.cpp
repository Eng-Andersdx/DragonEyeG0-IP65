#include <gui/containers/BigNumberCustomContainer.hpp>

BigNumberCustomContainer::BigNumberCustomContainer()
{

}

void BigNumberCustomContainer::updateValue(uint16_t newValue)
{
    if (newValue < 100)
    {
        Unicode::snprintf(valueTextAreaBuffer, VALUETEXTAREA_SIZE, "%d", newValue);
        invalidate();
    }
}

void BigNumberCustomContainer::updateColor(colortype newColor)
{
    valueTextArea.setColor(newColor);
    invalidate();
}

void BigNumberCustomContainer::initialize()
{
    BigNumberCustomContainerBase::initialize();
}
