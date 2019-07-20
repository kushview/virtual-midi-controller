#pragma once

#include "JuceHeader.h"

namespace vmc {

class LookAndFeel : public kv::LookAndFeel_KV1
{
public:
    LookAndFeel()
    {
        setColour (Slider::backgroundColourId, kv::LookAndFeel_KV1::widgetBackgroundColor.darker());
    }
};

}
