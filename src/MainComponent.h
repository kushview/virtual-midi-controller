
#pragma once

#include "JuceHeader.h"

namespace vmc {

class Controller;

class MainComponent   : public Component
{
public:
    MainComponent (Controller&);
    ~MainComponent();

    void paint (Graphics&) override;
    void resized() override;
    
private:
    Controller& controller;
    friend class Content; class Content;
    std::unique_ptr<Content> content;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

}
