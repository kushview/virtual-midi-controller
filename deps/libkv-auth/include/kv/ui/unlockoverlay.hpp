#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>

// Stub header for libkv-auth when not available
// This allows the project to build without the private libkv-auth dependency

namespace kv {

class UnlockOverlay : public juce::Component
{
public:
    template<typename StatusType>
    UnlockOverlay(StatusType& status, const juce::String& message) 
        : unlockStatus(&status), overlayMessage(message) {}
    
    ~UnlockOverlay() = default;
    
    std::function<void()> onDismissed;
    
    void paint(juce::Graphics& g) override
    {
        // Stub implementation - just show a message indicating unlock not available
        g.fillAll(juce::Colours::darkgrey.withAlpha(0.8f));
        g.setColour(juce::Colours::white);
        g.setFont(16.0f);
        g.drawText("Unlock functionality not available in this build", 
                   getLocalBounds(), juce::Justification::centred);
    }
    
private:
    void* unlockStatus; // Generic pointer since we don't know the exact type
    juce::String overlayMessage;
};

}