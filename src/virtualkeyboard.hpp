// Copyright 2025 (c) Kushview, LLC
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "juce.hpp"
#include <juce_audio_utils/juce_audio_utils.h>

namespace vmc {

// Custom MIDI keyboard component with aluminum theme styling
class MidiKeyboard : public juce::MidiKeyboardComponent {
public:
    MidiKeyboard (juce::MidiKeyboardState& state, Orientation orientation);

    // Override the drawing methods to match our aluminum theme
    void drawWhiteNote (int midiNoteNumber, juce::Graphics& g, juce::Rectangle<float> area,
                        bool isDown, bool isOver, juce::Colour lineColour, juce::Colour textColour) override;

    void drawBlackNote (int midiNoteNumber, juce::Graphics& g, juce::Rectangle<float> area,
                        bool isDown, bool isOver, juce::Colour noteFillColour) override;
};

class VirtualKeyboard : public Component {
public:
    VirtualKeyboard (MidiKeyboardState& state)
    {
        keyboard.reset (new MidiKeyboard (state, MidiKeyboard::horizontalKeyboard));
        addAndMakeVisible (keyboard.get());
        keyboard->setKeyWidth (32);
        keyboard->setScrollButtonWidth (22);
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

    MidiKeyboard& getMidiKeyboardComponent() { return *keyboard; }

private:
    std::unique_ptr<MidiKeyboard> keyboard;
};

} // namespace vmc
