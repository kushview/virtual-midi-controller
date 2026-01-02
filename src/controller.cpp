// Copyright 2025 (c) Kushview, LLC
// SPDX-License-Identifier: GPL-3.0-or-later

#include "controller.hpp"
#include "device.hpp"

using juce::File;
using juce::String;

namespace vmc {
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
