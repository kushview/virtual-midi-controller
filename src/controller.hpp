/*
    This file is part of Virtual MIDI Controller
    Copyright (c) 2019  Kushview, LLC.  All rights reserved.
*/

#pragma once

#include "juce.hpp"
#include "settings.hpp"

namespace vmc {

class Device;

class Controller final : public AudioIODeviceCallback,
                         public MidiInputCallback {
public:
    Controller();
    ~Controller();

    Device device() const;

    //=========================================================================
    Settings& getSettings();
    void saveSettings();

    //=========================================================================
    void addMidiMessage (const MidiMessage msg);
    MidiKeyboardState& getMidiKeyboardState();

    //=========================================================================
    static File getUserDataPath();
    static File getSamplesPath();

    //=========================================================================
    AudioDeviceManager& getDeviceManager();

    //=========================================================================
    void audioDeviceIOCallbackWithContext (const float* const* inputChannelData,
                                           int numInputChannels,
                                           float* const* outputChannelData,
                                           int numOutputChannels,
                                           int numSamples,
                                           const AudioIODeviceCallbackContext& context) override;
    void audioDeviceAboutToStart (AudioIODevice* device) override;
    void audioDeviceStopped() override;
    void audioDeviceError (const String& errorMessage) override;

    //=========================================================================
    virtual void handleIncomingMidiMessage (MidiInput*, const MidiMessage&) override {}
    virtual void handlePartialSysexMessage (MidiInput*, const uint8*,
                                            int, double) override {}

private:
    struct Impl;
    std::unique_ptr<Impl> impl;

    //=========================================================================
    friend class Application;
    void initializeAudioDevice();
    void shutdown();
};

} // namespace vmc
