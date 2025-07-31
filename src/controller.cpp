/*
    This file is part of Virtual MIDI Controller
    Copyright (c) 2019  Kushview, LLC.  All rights reserved.
*/

#include "controller.hpp"

namespace vmc
{

    struct Controller::Impl : public MidiKeyboardStateListener
    {
        Impl() {}
        ~Impl() {}

        Settings settings;
        OptionalScopedPointer<AudioDeviceManager> audioDeviceManager;
        std::unique_ptr<MidiOutput> midiOut;
        MidiKeyboardState keyboardState;
        juce::String virtualDeviceName {"VMC-MIDI-Out"};
        void init()
        {
            audioDeviceManager.setOwned(new AudioDeviceManager());
#if JUCE_MAC || JUCE_LINUX
            midiOut = MidiOutput::createNewDevice (virtualDeviceName);
            if (midiOut != nullptr)
                midiOut->startBackgroundThread();
#endif
            keyboardState.addListener(this);
        }

        void handleNoteOn(MidiKeyboardState *, int midiChannel, int midiNoteNumber, float velocity) override
        {
            if (midiOut)
                midiOut->sendMessageNow(MidiMessage::noteOn(midiChannel, midiNoteNumber, velocity));
            if (auto *dout = audioDeviceManager->getDefaultMidiOutput())
                dout->sendMessageNow(MidiMessage::noteOn(midiChannel, midiNoteNumber, velocity));
        }

        void handleNoteOff(MidiKeyboardState *, int midiChannel, int midiNoteNumber, float velocity) override
        {
            if (midiOut)
                midiOut->sendMessageNow(MidiMessage::noteOff(midiChannel, midiNoteNumber, velocity));
            if (auto *dout = audioDeviceManager->getDefaultMidiOutput())
                dout->sendMessageNow(MidiMessage::noteOff(midiChannel, midiNoteNumber, velocity));
        }
    };

    Controller::Controller()
    {
        impl.reset(new Impl());
        impl->init();
    }

    Controller::~Controller()
    {
        impl->keyboardState.removeListener(impl.get());
        if (impl->midiOut != nullptr)
            impl->midiOut->stopBackgroundThread();
        impl.reset();
    }

    void Controller::initializeAudioDevice()
    {
        auto &devices = getDeviceManager();
        auto &settings = impl->settings;
        bool initDefault = true;

        if (auto *const props = settings.getUserSettings())
        {
            if (auto xml = props->getXmlValue("devices"))
            {
                initDefault = devices.initialise(32, 32, xml.get(), false).isNotEmpty();
            }
        }

        if (initDefault)
        {
            devices.initialiseWithDefaultDevices(32, 32);
        }

        devices.addAudioCallback(this);
        devices.addMidiInputDeviceCallback(String(), this);
    }

    void Controller::shutdown()
    {
        auto &devices = getDeviceManager();
        devices.removeAudioCallback(this);
        devices.removeMidiInputDeviceCallback(String(), this);
        devices.closeAudioDevice();
    }

    void Controller::saveSettings()
    {
        auto &settings = impl->settings;
        auto &devices = getDeviceManager();

        if (auto *const props = settings.getUserSettings())
        {
            if (auto devicesXml = devices.createStateXml())
            {
                props->setValue("devices", devicesXml.get());
            }
        }
    }

    File Controller::getUserDataPath()
    {
        auto path = File::getSpecialLocation(File::userMusicDirectory).getChildFile("Controller");
        if (!path.exists())
            path.createDirectory();
        return path;
    }

    File Controller::getSamplesPath()
    {
        auto path = getUserDataPath().getChildFile("Samples");
        if (!path.exists())
            path.createDirectory();
        return path;
    }

    MidiKeyboardState &Controller::getMidiKeyboardState() { return impl->keyboardState; }
    Settings &Controller::getSettings() { return impl->settings; }
    AudioDeviceManager &Controller::getDeviceManager() { return *impl->audioDeviceManager; }

    void Controller::addMidiMessage(const MidiMessage msg)
    {
        // std::cout << msg.getDescription() << std::endl;
        
        if (impl->midiOut)
            impl->midiOut->sendMessageNow(msg);
        if (auto *const dout = impl->audioDeviceManager->getDefaultMidiOutput())
            dout->sendMessageNow(msg);
    }

    void Controller::audioDeviceIOCallbackWithContext(const float *const *inputChannelData,
                                                      int numInputChannels,
                                                      float *const *outputChannelData,
                                                      int numOutputChannels,
                                                      int numSamples,
                                                      const AudioIODeviceCallbackContext &context)
    {
        juce::ignoreUnused(inputChannelData, numInputChannels, outputChannelData, numOutputChannels, numSamples, context);
    }

    void Controller::audioDeviceAboutToStart(AudioIODevice *) {}
    void Controller::audioDeviceStopped() {}
    void Controller::audioDeviceError(const String &errorMessage) { juce::ignoreUnused(errorMessage); }

}
