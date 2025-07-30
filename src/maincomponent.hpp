/*
    This file is part of Virtual MIDI Controller
    Copyright (c) 2019  Kushview, LLC.  All rights reserved.
*/

#pragma once

#include "juce.hpp"
#include <juce_gui_basics/juce_gui_basics.h>

#include "unlock.hpp"
#include "midicceditor.hpp"

// Base UI dimensions - the keyboard area should stay this size
#ifndef VMC_WIDTH
#define VMC_WIDTH 900
#endif

#ifndef VMC_HEIGHT
#define VMC_HEIGHT 320
#endif

namespace vmc {

class Controller;
class MainComponent;

class CCDial : public juce::Slider {
public:
    CCDial (Controller&);

    void setControllerNumber (int ccNo) {
        _cc = juce::jlimit (0, 127, ccNo);
    }

    void setMidiChannel (int ch) {
        _channel = juce::jlimit (1, 16, ch);
    }

private:
    Controller& _controller;
    int _cc = 0, _channel = 1;
};

class MainComponent   : public Component
{
public:
    MainComponent (Controller&);
    ~MainComponent();

    void paint (Graphics&) override;
    void resized() override;
    
    void toggleCCEditor();
    
private:
    Controller& controller;
    friend class Content; class Content;
    std::unique_ptr<Content> content;
    std::unique_ptr<MidiCCDrawer> ccDrawer;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

}
