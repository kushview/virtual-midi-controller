#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

// Stub header for libkv-auth when not available
// This allows the project to build without the private libkv-auth dependency

namespace kv { namespace ui {

class UnlockForm : public juce::Component
{
public:
    UnlockForm() = default;
    ~UnlockForm() = default;
    
    void paint(juce::Graphics& g) override
    {
        // Stub implementation
        g.fillAll(juce::Colours::lightgrey);
        g.setColour(juce::Colours::darkgrey);
        g.setFont(14.0f);
        g.drawText("Unlock form not available in this build", 
                   getLocalBounds(), juce::Justification::centred);
    }
};

}}