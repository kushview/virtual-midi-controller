/*
    This file is part of Virtual MIDI Controller
    Copyright (c) 2019  Kushview, LLC.  All rights reserved.
*/

#include "maincomponent.hpp"
#include "virtualkeyboard.hpp"
#include "controller.hpp"

namespace vmc {
namespace detail {
    inline static void styleIncDecSlider (juce::Slider& s, bool readOnly = false) {
        s.setSliderStyle (Slider::IncDecButtons);
        s.setTextBoxStyle (Slider::TextBoxLeft, readOnly, 40, 24);
    }
}

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
                midiChannel, 60, value));
        };

        addAndMakeVisible (slider2);
        slider2.setRange (0.0, 127.0, 1.0);
        slider2.setSliderStyle (Slider::LinearVertical);
        slider2.onValueChange = [this]()
        {
            int value = roundToInt (slider2.getValue());
            owner.controller.addMidiMessage (MidiMessage::controllerEvent (
                midiChannel, 61, value));
        };

        addAndMakeVisible (slider3);
        slider3.setRange (0.0, 127.0, 1.0);
        slider3.setSliderStyle (Slider::LinearVertical);
        slider3.onValueChange = [this]()
        {
            int value = roundToInt (slider3.getValue());
            owner.controller.addMidiMessage (MidiMessage::controllerEvent (
                midiChannel, 62, value));
        };

        addAndMakeVisible (keyboard);
       
        addAndMakeVisible (program);
        detail::styleIncDecSlider (program);
        program.setTooltip ("MIDI Program");
        program.setRange (1.0, 128.0, 1.0);
        program.setValue (1.0, dontSendNotification);
        
        program.onValueChange = [this]()
        {
            int value = jlimit (1, 128, juce::roundToInt (program.getValue())) - 1;
            owner.controller.getSettings().set (Settings::lastMidiProgram, value);
            owner.controller.addMidiMessage (MidiMessage::programChange (midiChannel, value));
        };

        addAndMakeVisible (channel);
        detail::styleIncDecSlider (channel);
        channel.setTooltip ("MIDI Channel");
        channel.setRange (1.0, 16.0, 1.0);
        channel.setValue (1.0, dontSendNotification);
        channel.onValueChange = [this]()
        {
            midiChannel = juce::roundToInt (channel.getValue());
            owner.controller.getSettings().set (Settings::lastMidiChannel, midiChannel);
            keyboard.setMidiChannel (midiChannel);
        };

        addAndMakeVisible (output);
        output.setTooltip ("MIDI output device");
        output.onChange = [this]()
        {
            auto& devices = owner.controller.getDeviceManager();

            if (output.getSelectedId() == 1)
                devices.setDefaultMidiOutputDevice (String());
            else {
                const auto info = _devices [output.getSelectedId() - 1000];
                devices.setDefaultMidiOutputDevice (info.identifier);
            }
        };

        for (int i = 0; i < 8; ++i) {
            auto dial = _dials.add (new juce::Slider());
            dial->setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
            dial->setRange (0.0, 127.0, 1.0);
            dial->setTextBoxStyle (juce::Slider::NoTextBox, true, 10, 10);
            addAndMakeVisible (dial);
        }

        setSize (440, 340);

        updateMidiOutputs();
        updateWithSettings();
    }

    ~Content()
    {
        slider1.onValueChange = nullptr;
        slider2.onValueChange = nullptr;
        slider3.onValueChange = nullptr;
        program.onValueChange = nullptr;
        channel.onValueChange = nullptr;
        output.onChange = nullptr;
    }

    void updateWithSettings()
    {
        auto& settings = owner.controller.getSettings();

        midiChannel = settings.getInt (Settings::lastMidiChannel, 1);
        channel.setValue ((double) midiChannel, dontSendNotification);
        program.setValue (1.0 + (double) settings.getInt (Settings::lastMidiProgram, 0), dontSendNotification);

        const String ID = owner.controller.getDeviceManager().getDefaultMidiOutputIdentifier();
        if (ID.isEmpty())
        {
            output.setSelectedItemIndex (0, dontSendNotification);
        }
        else
        {
            for (int i = output.getNumItems(); --i >= 0;)
            {
                if (output.getItemText (i) == ID)
                {
                    output.setSelectedItemIndex (i, dontSendNotification);
                    break;
                }
            }
        }
    }

    void paint (Graphics& g) override
    {
        g.fillAll (Colours::darkgrey);
    }

    void resized() override
    {
        auto r = getLocalBounds().reduced(4);
        auto r2 = r.removeFromTop (22);
        channel.setBounds (r2.removeFromLeft (90));
        program.setBounds (r2.removeFromLeft (90));
        output.setBounds (r2.removeFromRight (140));

        auto r3 = r.removeFromBottom (180);
        slider1.setBounds (r3.removeFromLeft (30));
        slider2.setBounds (r3.removeFromLeft (30));
        r3.removeFromLeft (4);
        keyboard.setBounds (r3);

        int sw = r.getWidth() / _dials.size();
        for (auto* dial : _dials) {
            dial->setBounds (r.removeFromLeft (sw));
        }
    }

    void updateMidiOutputs()
    {
        _devices.clear();
        _devices = MidiOutput::getAvailableDevices();

        output.clear (dontSendNotification);
        output.addItem ("None", 1);
        output.addSeparator();

        int index = 0;
        for (const auto& info : _devices)
            output.addItem (info.name, 1000 + index);
    }

private:
    MainComponent& owner;
    VirtualKeyboard keyboard;
    Slider slider1, slider2, slider3;
    Slider program, channel;
    ComboBox output;

    juce::OwnedArray<juce::Slider> _dials;
    juce::Array<juce::MidiDeviceInfo> _devices;

    int midiChannel = 1;
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
