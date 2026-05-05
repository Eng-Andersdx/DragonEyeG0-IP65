#ifndef BIGNUMBERCUSTOMCONTAINER_HPP
#define BIGNUMBERCUSTOMCONTAINER_HPP

#include <gui_generated/containers/BigNumberCustomContainerBase.hpp>

class BigNumberCustomContainer : public BigNumberCustomContainerBase
{
public:
    BigNumberCustomContainer();
    virtual ~BigNumberCustomContainer() {}

    void updateValue(uint16_t newValue);
    void updateColor(colortype newColor);

    virtual void initialize();
protected:
};

#endif // BIGNUMBERCUSTOMCONTAINER_HPP
