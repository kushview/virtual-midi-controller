
#include "MainComponent.h"
#include "VirtualKeyboard.h"
#include "Controller.h"

namespace vmc {

class MainComponent::Content : public Component
{
public:
    Content (MainComponent& o)
        : owner (o),
          keyboard (o.controller.getMidiKeyboardState())
    {
        setOpaque (true);
        addAndMakeVisible (midiCCButton);
        midiCCButton.setButtonText ("MIDI CC 60 value 100");
        midiCCButton.onClick = [this]() {
            owner.controller.addMidiMessage (MidiMessage::controllerEvent (
                1, 60, 100
            ));
        };

        addAndMakeVisible (keyboard);
        setSize (440, 340);
    }

    ~Content()
    {

    }

    void paint (Graphics& g) override
    {
        g.fillAll (kv::LookAndFeel_KV1::widgetBackgroundColor.darker());
    }

    void resized() override
    {
        auto r = getLocalBounds();
        midiCCButton.setBounds (r.removeFromBottom (22));
        r.removeFromBottom(2);
        keyboard.setBounds (r);
    }


private:
    MainComponent& owner;
    VirtualKeyboard keyboard;
    TextButton midiCCButton;
};

MainComponent::MainComponent (Controller& vc)
    : controller (vc)
{
    setOpaque (true);
    content.reset (new Content (*this));
    addAndMakeVisible (content.get());
    setSize (600, 400);
}

MainComponent::~MainComponent()
{
    content.reset();
}

void MainComponent::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (DocumentWindow::backgroundColourId));
}

void MainComponent::resized()
{
    content->setBounds (getLocalBounds());
}

}
