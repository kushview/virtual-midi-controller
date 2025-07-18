/*
    This file is part of Virtual MIDI Controller
    Copyright (c) 2019  Kushview, LLC.  All rights reserved.
*/

#pragma once

#include "juce.hpp"
#include <juce_audio_utils/juce_audio_utils.h>

namespace vmc {

class VirtualKeyboard : public Component
{
public:
    VirtualKeyboard (MidiKeyboardState& state)
    {
        keyboard.reset (new MidiKeyboardComponent (state, MidiKeyboardComponent::horizontalKeyboard));
        addAndMakeVisible (keyboard.get());
    }

    ~VirtualKeyboard()
    {
        keyboard = nullptr;
    }

    /** Set the MIDI channel to use */
    void setMidiChannel (int channel) { keyboard->setMidiChannel (channel); }

    /** Get the current MIDI channel */
    int getMidiChannel() const noexcept { return keyboard->getMidiChannel(); }

    void resized() override
    {
        keyboard->setBounds (getLocalBounds());
    }

    MidiKeyboardComponent& getMidiKeyboardComponent() { return *keyboard; }

private:
    std::unique_ptr<MidiKeyboardComponent> keyboard;
};

}
