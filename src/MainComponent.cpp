
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

        addAndMakeVisible (slider1);
        slider1.setRange (0.0, 127.0, 1.0);
        slider1.setSliderStyle (Slider::LinearVertical);
        slider1.onValueChange = [this]()
        {
            int value = roundToInt (slider1.getValue());
            owner.controller.addMidiMessage (MidiMessage::controllerEvent (
                1, 60, value));
        };

        addAndMakeVisible (slider2);
        slider2.setRange (0.0, 127.0, 1.0);
        slider2.setSliderStyle (Slider::LinearVertical);
        slider2.onValueChange = [this]()
        {
            int value = roundToInt (slider2.getValue());
            owner.controller.addMidiMessage (MidiMessage::controllerEvent (
                1, 61, value));
        };

        addAndMakeVisible (slider3);
        slider3.setRange (0.0, 127.0, 1.0);
        slider3.setSliderStyle (Slider::LinearVertical);
        slider3.onValueChange = [this]()
        {
            int value = roundToInt (slider3.getValue());
            owner.controller.addMidiMessage (MidiMessage::controllerEvent (
                1, 62, value));
        };

        addAndMakeVisible (keyboard);
        
        addAndMakeVisible (program);
        for (int i = 0; i < 128; ++i)
            program.addItem (String (i), 1 + i);
        program.onChange = [this]()
        {
            int value = jlimit (0, 127, program.getSelectedItemIndex());
            owner.controller.addMidiMessage (MidiMessage::programChange (1, value));
        };

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
        slider1.setBounds (r.removeFromLeft (18));
        slider2.setBounds (r.removeFromLeft (18));
        slider3.setBounds (r.removeFromRight (18));
        program.setBounds (r.removeFromBottom (18));
        keyboard.setBounds (r);
    }

private:
    MainComponent& owner;
    VirtualKeyboard keyboard;
    Slider slider1, slider2, slider3;
    ComboBox program;
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
