
#pragma once

#include "JuceHeader.h"

namespace vmc {

class VirtualKeyboard : public MidiKeyboardComponent
{
public:
    VirtualKeyboard (MidiKeyboardState& state)
        : MidiKeyboardComponent (state, MidiKeyboardComponent::horizontalKeyboard)
    { }

    ~VirtualKeyboard() { }
};

}
