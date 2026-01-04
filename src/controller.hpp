// Copyright 2025 (c) Kushview, LLC
// SPDX-License-Identifier: GPL-3.0-or-later

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

    struct Listener {
        virtual ~Listener() = default;
        virtual void deviceChanged() = 0;
    };

    Device device() const;
    bool loadDeviceFile (const juce::File&);
    File deviceFile() const noexcept;

    //=========================================================================
    Settings& getSettings();
    void saveSettings();
    void restoreSettings();

    //=========================================================================
    void addMidiMessage (const MidiMessage msg);
    MidiKeyboardState& getMidiKeyboardState();

    //=========================================================================
    static File getUserDataPath();
    static File getSamplesPath();

    //=========================================================================
    AudioDeviceManager& getDeviceManager();

    /** Call this after changing the default MIDI output device. */
    void updateMidiOutput();

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

    void addListener (Listener*);
    void removeListener (Listener*);

private:
    struct Impl;
    std::unique_ptr<Impl> impl;

    //=========================================================================
    friend class Application;
    void initializeAudioDevice();
    void shutdown();
};

} // namespace vmc
