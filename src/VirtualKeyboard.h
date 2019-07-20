
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

    /** Set the MIDI channel to use */
    void setMidiChannel (int channel) { keyboard->setMidiChannel (channel); }

    /** Get the current MIDI channel */
    int getMidiChannel() const noexcept { return keyboard->getMidiChannel(); }

    void resized() override
    {
        keyboard->setBounds (getLocalBounds());
    }

private:
    std::unique_ptr<MidiKeyboardComponent> keyboard;
};

}
