#pragma once

#include "JuceHeader.h"
#include "Settings.h"

namespace vmc {

class Controller final : public AudioIODeviceCallback,
                         public MidiInputCallback
{
public:
    Controller();
    ~Controller();

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
    void audioDeviceIOCallback (const float** inputChannelData,
                                int numInputChannels, float** outputChannelData,
                                int numOutputChannels, int numSamples) override;
    void audioDeviceAboutToStart (AudioIODevice* device) override;
    void audioDeviceStopped() override;
    void audioDeviceError (const String& errorMessage) override;

    virtual void handleIncomingMidiMessage (MidiInput*, const MidiMessage&) override {}
    virtual void handlePartialSysexMessage (MidiInput*, const uint8*,
                                            int, double) override {}

private:
    struct Impl; std::unique_ptr<Impl> impl;

    //=========================================================================
    friend class Application;
    void initializeAudioDevice();
    void shutdown();
};

}
