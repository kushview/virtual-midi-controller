// Copyright 2025 (c) Kushview, LLC
// SPDX-License-Identifier: GPL-3.0-or-later

#include "controller.hpp"
#include "device.hpp"

using juce::File;
using juce::String;

namespace vmc {

/** Listens to Device ValueTree data and generates MIDI messages when values change. */
class MidiDispatcher : public juce::ValueTree::Listener {
public:
    /** Callback function type for receiving generated MIDI messages. */
    using MidiCallback = std::function<void (const MidiMessage&)>;

    MidiDispatcher() = default;
    ~MidiDispatcher() override { detach(); }

    /** Attaches the dispatcher to a Device's data and starts listening for changes.
        @param device The device to monitor for changes.
        @param callback The callback function to invoke when MIDI messages are generated.
    */
    void attach (Device& device, MidiCallback callback)
    {
        detach();
        _data = device.data();
        _midiCallback = std::move (callback);
        _data.addListener (this);
    }

    /** Detaches from the current device and stops listening. */
    void detach()
    {
        if (_data.isValid())
            _data.removeListener (this);
        _data = juce::ValueTree();
        _midiCallback = nullptr;
    }

    /** Returns true if currently attached to a device. */
    bool isAttached() const noexcept { return _data.isValid() && _midiCallback != nullptr; }

private:
    juce::ValueTree _data;
    MidiCallback _midiCallback;

    void sendMidiMessage (const MidiMessage& msg)
    {
        if (_midiCallback)
            _midiCallback (msg);
    }

    int getMidiChannel() const noexcept
    {
        return _data.getProperty (Device::midiChannelID, 1);
    }

    void valueTreePropertyChanged (juce::ValueTree& tree, const juce::Identifier& property) override
    {
        if (! _midiCallback)
            return;

        // Handle device-level property changes
        if (tree == _data) {
            if (property == Device::midiProgramID) {
                int program = static_cast<int> (tree.getProperty (property)) - 1; // MIDI programs are 0-127
                program = juce::jlimit (0, 127, program);
                sendMidiMessage (MidiMessage::programChange (getMidiChannel(), program));
            }
            return;
        }

        // Handle dial/fader value changes (Ranged children)
        if (tree.getType() == Device::RangedID && property == Device::valueID) {
            auto parent = tree.getParent();
            if (parent.isValid() && (parent.getType() == Device::dialsID || parent.getType() == Device::fadersID)) {
                int ccNumber = tree.getProperty (Device::ccNumberID, 0);
                int value = static_cast<int> (tree.getProperty (Device::valueID));
                value = juce::jlimit (0, 127, value);
                sendMidiMessage (MidiMessage::controllerEvent (getMidiChannel(), ccNumber, value));
            }
        }
    }

    void valueTreeChildAdded (juce::ValueTree&, juce::ValueTree&) override {}
    void valueTreeChildRemoved (juce::ValueTree&, juce::ValueTree&, int) override {}
    void valueTreeChildOrderChanged (juce::ValueTree&, int, int) override {}
    void valueTreeParentChanged (juce::ValueTree&) override {}
    void valueTreeRedirected (juce::ValueTree&) override {}
};

struct Controller::Impl : public MidiKeyboardStateListener {
    Impl (Controller& c) : owner (c) {}
    ~Impl() {}

    Controller& owner;
    Settings settings;
    OptionalScopedPointer<AudioDeviceManager> audioDeviceManager;
    std::unique_ptr<MidiOutput> midiOut;
    MidiKeyboardState keyboardState;
    juce::String virtualDeviceName { "VMC-MIDI-Out" };
    Device device;
    juce::File deviceFile;
    ListenerList<Controller::Listener> listeners;
    MidiDispatcher dispatch;

    void saveSettings()
    {
        auto& devices = owner.getDeviceManager();

        if (auto* const props = settings.getUserSettings()) {
            if (auto devicesXml = devices.createStateXml()) {
                props->setValue ("devices", devicesXml.get());
            }
            if (deviceFile != File() && deviceFile.existsAsFile())
                props->setValue ("lastDeviceFile", deviceFile.getFullPathName());
        }
    }

    void restoreSettings()
    {
        if (auto* props = settings.getUserSettings()) {
            const auto path = props->getValue ("lastDeviceFile");
            if (File::isAbsolutePath (path)) {
                auto fileToLoad = File (path);
                if (loadDeviceFile (fileToLoad)) {
                    deviceFile = fileToLoad;
                }
            }
        }
    }

    bool loadDeviceFile (const juce::File& file)
    {
        if (device.load (file)) {
            deviceFile = file;
            listeners.call (&Controller::Listener::deviceChanged);
            dispatch.attach (device, [this] (const MidiMessage& msg) {
                owner.addMidiMessage (msg);
            });
            return true;
        }
        return false;
    }

    void init()
    {
        audioDeviceManager.setOwned (new AudioDeviceManager());
#if JUCE_MAC || JUCE_LINUX
        midiOut = MidiOutput::createNewDevice (virtualDeviceName);
        if (midiOut != nullptr)
            midiOut->startBackgroundThread();
#endif
        keyboardState.addListener (this);
    }

    void shutdown()
    {
        dispatch.detach();
        if (deviceFile != File() && deviceFile.existsAsFile())
            device.save (deviceFile);
    }

    void handleNoteOn (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override
    {
        owner.addMidiMessage (MidiMessage::noteOn (midiChannel, midiNoteNumber, velocity));
    }

    void handleNoteOff (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override
    {
        owner.addMidiMessage (MidiMessage::noteOff (midiChannel, midiNoteNumber, velocity));
    }
};

Controller::Controller()
{
    impl.reset (new Impl (*this));
    impl->init();
}

Controller::~Controller()
{
    impl->keyboardState.removeListener (impl.get());
    if (impl->midiOut != nullptr)
        impl->midiOut->stopBackgroundThread();
    impl.reset();
}

void Controller::initializeAudioDevice()
{
    auto& devices = getDeviceManager();
    auto& settings = impl->settings;
    bool initDefault = true;

    if (auto* const props = settings.getUserSettings()) {
        if (auto xml = props->getXmlValue ("devices")) {
            initDefault = devices.initialise (32, 32, xml.get(), false).isNotEmpty();
        }
    }

    if (initDefault) {
        devices.initialiseWithDefaultDevices (32, 32);
    }

    devices.addAudioCallback (this);
    devices.addMidiInputDeviceCallback (String(), this);
}

void Controller::shutdown()
{
    impl->shutdown();
    auto& devices = getDeviceManager();
    devices.removeAudioCallback (this);
    devices.removeMidiInputDeviceCallback (String(), this);
    devices.closeAudioDevice();
}

void Controller::saveSettings() { impl->saveSettings(); }
void Controller::restoreSettings() { impl->restoreSettings(); }

File Controller::getUserDataPath()
{
    auto path = File::getSpecialLocation (File::userMusicDirectory).getChildFile ("Controller");
    if (! path.exists())
        path.createDirectory();
    return path;
}

File Controller::getSamplesPath()
{
    auto path = getUserDataPath().getChildFile ("Samples");
    if (! path.exists())
        path.createDirectory();
    return path;
}

MidiKeyboardState& Controller::getMidiKeyboardState() { return impl->keyboardState; }
Device Controller::device() const { return impl->device; }
bool Controller::loadDeviceFile (const juce::File& file) { return impl->loadDeviceFile (file); }
File Controller::deviceFile() const noexcept { return impl->deviceFile; }

Settings& Controller::getSettings() { return impl->settings; }
AudioDeviceManager& Controller::getDeviceManager() { return *impl->audioDeviceManager; }

void Controller::addMidiMessage (const MidiMessage msg)
{
    // std::cout << msg.getDescription() << std::endl;

    if (impl->midiOut)
        impl->midiOut->sendMessageNow (msg);
    if (auto* const dout = impl->audioDeviceManager->getDefaultMidiOutput())
        dout->sendMessageNow (msg);
}

void Controller::audioDeviceIOCallbackWithContext (const float* const* inputChannelData,
                                                   int numInputChannels,
                                                   float* const* outputChannelData,
                                                   int numOutputChannels,
                                                   int numSamples,
                                                   const AudioIODeviceCallbackContext& context)
{
    juce::ignoreUnused (inputChannelData, numInputChannels, outputChannelData, numOutputChannels, numSamples, context);
}

void Controller::audioDeviceAboutToStart (AudioIODevice*) {}
void Controller::audioDeviceStopped() {}
void Controller::audioDeviceError (const String& errorMessage) { juce::ignoreUnused (errorMessage); }

void Controller::addListener (Listener* listener) { impl->listeners.add (listener); }
void Controller::removeListener (Listener* listener) { impl->listeners.remove (listener); }

} // namespace vmc
