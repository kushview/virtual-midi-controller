/*
    This file is part of Virtual MIDI Controller
    Copyright (c) 2019  Kushview, LLC.  All rights reserved.
*/

#pragma once

#include "juce.hpp"

namespace vmc {

#if HAVE_KV
class LookAndFeel : public kv::LookAndFeel_KV1
{
public:
    LookAndFeel()
    {
        setColour (Slider::backgroundColourId, kv::LookAndFeel_KV1::widgetBackgroundColor.darker());
    }
};
#else
class LookAndFeel : public juce::LookAndFeel_V4
{
public:
    LookAndFeel() = default;
};
#endif

}
