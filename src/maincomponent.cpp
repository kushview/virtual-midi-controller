/*
    This file is part of Virtual MIDI Controller
    Copyright (c) 2019  Kushview, LLC.  All rights reserved.
*/

#include "maincomponent.hpp"
#include "virtualkeyboard.hpp"
#include "controller.hpp"
#include "BinaryData.h"

namespace vmc {
namespace detail {
inline static void styleIncDecSlider (juce::Slider& s, bool readOnly = false)
{
    s.setSliderStyle (Slider::IncDecButtons);
    s.setTextBoxStyle (Slider::TextBoxLeft, readOnly, 40, 24);
}
} // namespace detail

CCDial::CCDial (Controller& c) : _controller (c)
{
    setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    setRange (0.0, 127.0, 1.0);
    setTextBoxStyle (juce::Slider::NoTextBox, true, 10, 10);

    onValueChange = [this]() {
        int value = juce::roundToInt (getValue());
        _controller.addMidiMessage (MidiMessage::controllerEvent (
            _channel, _cc, value));
    };
}

class MainComponent::Content : public Component {
public:
    Content (MainComponent& o)
        : owner (o),
          keyboard (o.controller.getMidiKeyboardState())
    {
        // Load the logo image
        logo = juce::ImageCache::getFromMemory (BinaryData::vmclogo_png,
                                                BinaryData::vmclogo_pngSize);

        setOpaque (true);

        // Generate horizontal and vertical brush patterns
        juce::Random random;
        brushAlphas.resize (2048);
        verticalBrushAlphas.resize (2048);
        for (int i = 0; i < brushAlphas.size(); ++i) {
            brushAlphas[i] = random.nextFloat() * 0.02f + 0.02f;           // reduced from 0.03f
            verticalBrushAlphas[i] = random.nextFloat() * 0.015f + 0.015f; // reduced from 0.02f
        }

        addAndMakeVisible (slider1);
        slider1.setRange (0.0, 127.0, 1.0);
        slider1.setSliderStyle (Slider::LinearVertical);
        slider1.onValueChange = [this]() {
            int value = roundToInt (slider1.getValue());
            owner.controller.addMidiMessage (MidiMessage::controllerEvent (
                midiChannel, 60, value));
        };

        addAndMakeVisible (slider2);
        slider2.setRange (0.0, 127.0, 1.0);
        slider2.setSliderStyle (Slider::LinearVertical);
        slider2.onValueChange = [this]() {
            int value = roundToInt (slider2.getValue());
            owner.controller.addMidiMessage (MidiMessage::controllerEvent (
                midiChannel, 61, value));
        };

        addAndMakeVisible (slider3);
        slider3.setRange (0.0, 127.0, 1.0);
        slider3.setSliderStyle (Slider::LinearVertical);
        slider3.onValueChange = [this]() {
            int value = roundToInt (slider3.getValue());
            owner.controller.addMidiMessage (MidiMessage::controllerEvent (
                midiChannel, 62, value));
        };

        addAndMakeVisible (keyboard);

        // Add a button to toggle the CC editor
        addAndMakeVisible (ccEditorButton);
        ccEditorButton.setButtonText ("CC Editor");
        ccEditorButton.setClickingTogglesState (true);

        // Set text color to be visible on the aluminum background
        ccEditorButton.setColour (juce::TextButton::textColourOffId, juce::Colours::white.withAlpha (0.8f));
        ccEditorButton.setColour (juce::TextButton::textColourOnId, juce::Colours::white);
        ccEditorButton.setColour (juce::TextButton::buttonOnColourId, juce::Colour::fromRGB (64, 160, 255));

        ccEditorButton.onClick = [this]() {
            owner.toggleDrawer();
        };

        addAndMakeVisible (program);
        detail::styleIncDecSlider (program);
        program.setTooltip ("MIDI Program");
        program.setRange (1.0, 128.0, 1.0);
        program.setValue (1.0, dontSendNotification);

        program.onValueChange = [this]() {
            int value = jlimit (1, 128, juce::roundToInt (program.getValue())) - 1;
            owner.controller.getSettings().set (Settings::lastMidiProgram, value);
            owner.controller.addMidiMessage (MidiMessage::programChange (midiChannel, value));
        };

        addAndMakeVisible (channel);
        detail::styleIncDecSlider (channel);
        channel.setTooltip ("MIDI Channel");
        channel.setRange (1.0, 16.0, 1.0);
        channel.setValue (1.0, dontSendNotification);
        channel.onValueChange = [this]() {
            midiChannel = juce::roundToInt (channel.getValue());
            owner.controller.getSettings().set (Settings::lastMidiChannel, midiChannel);
            keyboard.setMidiChannel (midiChannel);
            for (auto* d : _dials)
                d->setMidiChannel (midiChannel);
        };

        addAndMakeVisible (output);
        output.setTooltip ("MIDI output device");
        output.onChange = [this]() {
            auto& devices = owner.controller.getDeviceManager();

            if (output.getSelectedId() == 1)
                devices.setDefaultMidiOutputDevice (String());
            else {
                const auto info = _devices[output.getSelectedId() - 1000];
                devices.setDefaultMidiOutputDevice (info.identifier);
            }
        };

        int midiCC = 102; // start CC number here.
        for (int i = 0; i < 8; ++i) {
            auto dial = _dials.add (new CCDial (owner.controller));
            addAndMakeVisible (dial);
            dial->setName (String ("Control ") + String (i + 1));
            dial->setTooltip (dial->getName());
            dial->setControllerNumber (midiCC++);
            dial->setMidiChannel (midiChannel);
        }

        setSize (VMC_WIDTH, VMC_HEIGHT);
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

        const String ID = owner.controller.getDeviceManager().getDefaultMidiOutputIdentifier();
        if (ID.isEmpty()) {
            output.setSelectedItemIndex (0, dontSendNotification);
        } else {
            bool foundDevice = false;
            for (int i = 0; i < _devices.size(); ++i) {
                if (_devices[i].identifier == ID) {
                    output.setSelectedId (1000 + i, dontSendNotification);
                    foundDevice = true;
                    break;
                }
            }

            // If no matching device was found, default to "None"
            if (! foundDevice) {
                output.setSelectedItemIndex (0, dontSendNotification);
            }
        }

        if (settings.getValue (Settings::currentDrawer) == "ccEditor") {
            juce::Component::SafePointer<MainComponent> ptr (&this->owner);
            juce::Timer::callAfterDelay (14, [ptr] {
                if (ptr && ptr.getComponent() != nullptr)
                    if (ptr->ccDrawer != nullptr && ! ptr->ccDrawer->isOpen())
                        ptr->toggleDrawer();
            });
        }
    }

    void paint (Graphics& g) override
    {
        auto bounds = getLocalBounds();

        // Base aluminum color
        g.setColour (juce::Colour::fromRGB (45, 48, 52));
        g.fillRect (bounds);

        // Subtle radial gradient for depth (lighter center)
        juce::ColourGradient grad (
            juce::Colour::fromRGB (52, 55, 58), getWidth() * 0.5f, getHeight() * 0.5f, juce::Colour::fromRGB (42, 45, 48), 0.0f, 0.0f, true);
        g.setGradientFill (grad);
        g.fillRect (bounds);

        // Draw horizontal lines of the mesh
        for (int i = 0; i < getHeight(); i += 2) {
            g.setColour (juce::Colours::white.withAlpha (brushAlphas[i % brushAlphas.size()]));
            g.drawHorizontalLine (i, 0, (float) getWidth());
        }

        // Draw vertical lines of the mesh
        for (int i = 0; i < getWidth(); i += 2) {
            g.setColour (juce::Colours::white.withAlpha (verticalBrushAlphas[i % verticalBrushAlphas.size()]));
            g.drawVerticalLine (i, 0, (float) getHeight());
        }

        // Subtle outline that blends with the brush effect
        g.setColour (juce::Colours::black.withAlpha (0.15f));
        g.drawRect (bounds);

        // Draw logo centered at top
        if (logo.isValid()) {
            const int logoHeight = 40;
            const int logoWidth = (int) ((float) logoHeight * logo.getWidth() / logo.getHeight());
            logo = logo.rescaled (logoWidth, logoHeight, juce::Graphics::ResamplingQuality::highResamplingQuality);
            g.drawImageWithin (logo,
                               bounds.getCentreX() - logoWidth / 2,
                               4, // Top margin
                               logoWidth,
                               logoHeight,
                               juce::RectanglePlacement::centred,
                               false);
        }
    }

    void resized() override
    {
        auto r = getLocalBounds().reduced (4);
        auto r2 = r.removeFromTop (22);
        channel.setBounds (r2.removeFromLeft (90));
        program.setBounds (r2.removeFromLeft (90));
        r2.removeFromLeft (10); // Small gap
        ccEditorButton.setBounds (r2.removeFromLeft (80));
        output.setBounds (r2.removeFromRight (140));

        auto r3 = r.removeFromBottom (180);
        slider1.setBounds (r3.removeFromLeft (30));
        slider2.setBounds (r3.removeFromLeft (30));
        r3.removeFromLeft (4);
        keyboard.setBounds (r3);

        int sw = r.getWidth() / _dials.size();
        int swIndent = 16;
        for (auto* dial : _dials) {
            auto swr = r.removeFromLeft (sw);
            dial->setBounds (swr.reduced (swIndent));
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

    void setDevice (const Device& newDev)
    {
        if (device == newDev)
            return;
        device = newDev;
        if (device.isValid()) {
            midiChannelValue = device.propertyAsValue (Device::midiChannelID);
            midiProgramValue = device.propertyAsValue (Device::midiProgramID);
            channel.getValueObject().referTo (midiChannelValue);
            program.getValueObject().referTo (midiProgramValue);

            if (dialValues.size() != (size_t) _dials.size())
                dialValues.resize (_dials.size());

            const auto dialsTree = device.dials();
            for (int i = 0; i < _dials.size(); ++i) {
                auto child = dialsTree.getChild (i);
                if (child.isValid()) {
                    dialValues[(size_t) i] = child.getPropertyAsValue ("value", nullptr);
                    _dials.getUnchecked (i)->getValueObject().referTo (dialValues[(size_t) i]);
                }
            }

            if (faderValues.size() != 3u)
                faderValues.resize (3u);

            juce::Slider* faders[] = { &slider1, &slider2, &slider3 };
            const auto fadersTree = device.faders();
            for (int i = 0; i < 3; ++i) {
                auto child = fadersTree.getChild (i);
                if (child.isValid()) {
                    faderValues[(size_t) i] = child.getPropertyAsValue ("value", nullptr);
                    faders[i]->getValueObject().referTo (faderValues[(size_t) i]);
                }
            }
        }
    }

private:
    friend class MainComponent;
    MainComponent& owner;
    VirtualKeyboard keyboard;
    Slider slider1, slider2, slider3;
    Slider program, channel;
    ComboBox output;
    juce::TextButton ccEditorButton;
    Device device;
    juce::Value midiChannelValue;
    juce::Value midiProgramValue;
    std::vector<juce::Value> dialValues;
    std::vector<juce::Value> faderValues;

    juce::OwnedArray<CCDial> _dials;
    juce::Array<juce::MidiDeviceInfo> _devices;

    int midiChannel = 1;
    std::vector<float> brushAlphas;         // Store horizontal brush pattern
    std::vector<float> verticalBrushAlphas; // Store vertical brush pattern
    juce::Image logo;
};

MainComponent::MainComponent (Controller& vc)
    : controller (vc)
{
    status.load();

    setOpaque (true);
    content.reset (new Content (*this));
    addAndMakeVisible (content.get());

    // Create and add the CC drawer
    ccDrawer.reset (new MidiCCDrawer (vc));
    addAndMakeVisible (ccDrawer.get());

    // Set up callback to resize window when drawer size changes
    ccDrawer->onSizeChanged = [this] (int drawerHeight) {
        // The main content should stay at its base size (VMC_HEIGHT)
        // Only the total window size should change
        int totalHeight = VMC_HEIGHT + drawerHeight;
        setSize (VMC_WIDTH, totalHeight);
    };

    overlay = std::make_unique<kv::UnlockOverlay> (status, TRANS ("Unlock Virtual MIDI Keyboard"));
    // addAndMakeVisible (overlay.get());
    overlay->onDismissed = [this]() {
        overlay->setVisible (! status.isUnlocked());
        status.save();
        resized();
    };
    // overlay->setVisible (! status.isUnlocked());


    // Set initial size to the base UI dimensions
    setSize (VMC_WIDTH, VMC_HEIGHT);

    Timer::callAfterDelay(50, [this]() {
        content->updateMidiOutputs();
        content->updateWithSettings();
        content->setDevice (controller.device());
    });
}

MainComponent::~MainComponent()
{
    auto& settings = controller.getSettings();

    const auto midiCCStr = [this]() -> String {
        StringArray out;
        for (auto* dial : content->_dials) {
            out.add (String (dial->controllerNumber()));
        }
        return out.joinIntoString (",");
    }();

    if (auto* props = settings.getUserSettings()) {
        props->setValue (Settings::dialMidiCCs, midiCCStr);
        props->setValue (Settings::currentDrawer, ccDrawer->isOpen() ? "ccEditor" : "");
    }

    status.save();
    ccDrawer.reset();
    content.reset();
    overlay.reset();
}

void MainComponent::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (DocumentWindow::backgroundColourId));
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();

    // The main content should always maintain the base UI size (VMC_HEIGHT)
    auto contentBounds = bounds.removeFromTop (VMC_HEIGHT);
    content->setBounds (contentBounds);

    // The drawer gets any remaining space at the bottom
    if (ccDrawer) {
        if (bounds.getHeight() > 0) {
            ccDrawer->setBounds (bounds);
        } else {
            // Position the drawer even when closed (height 0)
            ccDrawer->setBounds (bounds.getX(), contentBounds.getBottom(), bounds.getWidth(), 0);
        }
    }

    if (overlay != nullptr && overlay->isVisible()) {
        overlay->setBounds (getLocalBounds());
    }
}

Device MainComponent::device() const
{
    return content->device;
}

void MainComponent::setDevice (const Device& newDevice)
{
    content->setDevice (newDevice);
}

void MainComponent::toggleDrawer()
{
    if (ccDrawer) {
        ccDrawer->toggleDrawer();

        // Populate the mappings when opening
        if (ccDrawer->isOpen()) {
            auto& editor = ccDrawer->getEditor();
            editor.refreshMappings();

            // Add mappings for all UI components
            if (content) {
                // Access the private Content class to get component references
                // This is a simplified approach - in a real implementation you'd want
                // a cleaner way to access these components

                // Add dials (we know there are 8 from the code)
                for (auto* dial : content->_dials) {
                    editor.addMapping (dial->getName(), dial, MidiCCMapping::Dial);
                }
            }
        }
    }
}
} // namespace vmc
