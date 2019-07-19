
#pragma once

#include "JuceHeader.h"

namespace vmc {

class VirtualKeyboard : public Component
{
public:
    VirtualKeyboard (MidiKeyboardState& state)
    {
        keyboard.reset (new MidiKeyboardComponent (state, MidiKeyboardComponent::horizontalKeyboard));
        addAndMakeVisible (keyboard.get());
    }

    ~VirtualKeyboard() { }

    void resized() override
    {
        keyboard->setBounds (getLocalBounds());
    }

private:
    std::unique_ptr<MidiKeyboardComponent> keyboard;
};

}
