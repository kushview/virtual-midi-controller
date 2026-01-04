// Copyright 2025 (c) Kushview, LLC
// SPDX-License-Identifier: GPL-3.0-or-later

#include "controller.hpp"
#include "device.hpp"

using juce::File;
using juce::String;

namespace vmc {

/** Generates sample-accurate MIDI clock messages.
    
    This engine calculates the correct sample positions for 24 PPQN MIDI clock
    messages based on BPM and sample rate. It's designed to be called from the
    audio callback for precise timing.
*/
class MidiClockEngine {
public:
    MidiClockEngine() = default;

    /** Sets the BPM and recalculates timing. Thread-safe for audio thread. */
    void setBpm (double newBpm) noexcept
    {
        bpm.store (juce::jlimit (20.0, 900.0, newBpm));
        recalculateSamplesPerTick();
    }

    double getBpm() const noexcept { return bpm.load(); }

    /** Called when audio device starts. Recalculates timing for new sample rate. */
    void prepare (double newSampleRate) noexcept
    {
        sampleRate = newSampleRate;
        recalculateSamplesPerTick();
        reset();
    }

    /** Resets the clock position to zero. */
    void reset() noexcept
    {
        samplePosition = 0.0;
    }

    /** Returns true if the clock is currently running. */
    bool isRunning() const noexcept { return running.load(); }

    /** Starts the clock. Returns the MIDI Start message (0xFA) to be sent. */
    MidiMessage start() noexcept
    {
        reset();
        running.store (true);
        return MidiMessage::midiStart();
    }

    /** Stops the clock. Returns the MIDI Stop message (0xFC) to be sent. */
    MidiMessage stop() noexcept
    {
        running.store (false);
        return MidiMessage::midiStop();
    }

    /** Generates clock events for the given block of samples.
        @param buffer The MidiBuffer to add clock messages to
        @param numSamples The number of samples in this block
    */
    void processBlock (juce::MidiBuffer& buffer, int numSamples)
    {
        if (! running.load() || samplesPerTick <= 0.0)
            return;

        for (int i = 0; i < numSamples; ++i) {
            if (samplePosition >= samplesPerTick) {
                buffer.addEvent (MidiMessage::midiClock(), i);
                samplePosition -= samplesPerTick;
            }
            samplePosition += 1.0;
        }
    }

private:
    std::atomic<double> bpm { 120.0 };
    std::atomic<bool> running { false };
    double sampleRate { 44100.0 };
    double samplesPerTick { 0.0 };
    double samplePosition { 0.0 };

    void recalculateSamplesPerTick() noexcept
    {
        // 24 PPQN (pulses per quarter note)
        // samplesPerTick = (sampleRate * 60.0) / (bpm * 24.0)
        const double currentBpm = bpm.load();
        if (currentBpm > 0.0 && sampleRate > 0.0)
            samplesPerTick = (sampleRate * 60.0) / (currentBpm * 24.0);
    }
};

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

struct Controller::Impl : public MidiKeyboardStateListener,
                          public juce::ValueTree::Listener {
    Impl (Controller& c) : owner (c) {}
    ~Impl() {}

    Controller& owner;
    Settings settings;
    OptionalScopedPointer<AudioDeviceManager> audioDeviceManager;
    std::unique_ptr<MidiOutput> virtualMidiOut;  // Virtual MIDI port (macOS/Linux)
    std::unique_ptr<MidiOutput> selectedMidiOut; // User-selected MIDI output device
    MidiKeyboardState keyboardState;
    juce::String virtualDeviceName { "VMC-MIDI-Out" };
    Device device;
    juce::File deviceFile;
    ListenerList<Controller::Listener> listeners;
    MidiDispatcher dispatch;
    MidiClockEngine clockEngine;
    juce::ValueTree deviceData;

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
            attachDevice();
            return true;
        }
        return false;
    }

    void init()
    {
        audioDeviceManager.setOwned (new AudioDeviceManager());
#if JUCE_MAC || JUCE_LINUX
        virtualMidiOut = MidiOutput::createNewDevice (virtualDeviceName);
        if (virtualMidiOut != nullptr)
            virtualMidiOut->startBackgroundThread();
#endif
        keyboardState.addListener (this);

        // Attach the default device to the dispatcher
        attachDevice();
    }

    void attachDevice()
    {
        dispatch.attach (device, [this] (const MidiMessage& msg) {
            owner.addMidiMessage (msg);
        });

        // Listen for clock property changes
        if (deviceData.isValid())
            deviceData.removeListener (this);
        deviceData = device.data();
        deviceData.addListener (this);

        // Sync clock engine with device settings
        clockEngine.setBpm (device.clockBpm());
        if (device.clockEnabled() && ! clockEngine.isRunning()) {
            ensureAudioDeviceRunning();
            owner.addMidiMessage (clockEngine.start());
        } else if (! device.clockEnabled() && clockEngine.isRunning()) {
            owner.addMidiMessage (clockEngine.stop());
        }
    }

    void shutdown()
    {
        if (deviceData.isValid())
            deviceData.removeListener (this);
        deviceData = juce::ValueTree();
        dispatch.detach();
        if (deviceFile != File() && deviceFile.existsAsFile())
            device.save (deviceFile);

        if (selectedMidiOut) {
            selectedMidiOut->stopBackgroundThread();
            selectedMidiOut.reset();
        }
    }

    void updateDefaultMidiOutput()
    {
        const auto newId = audioDeviceManager->getDefaultMidiOutputIdentifier();

        // Check if we need to update
        if (selectedMidiOut && selectedMidiOut->getIdentifier() == newId)
            return;

        // Stop old output
        if (selectedMidiOut) {
            selectedMidiOut->stopBackgroundThread();
            selectedMidiOut.reset();
        }

        // Open new output with background thread
        if (newId.isNotEmpty()) {
            selectedMidiOut = MidiOutput::openDevice (newId);
            if (selectedMidiOut)
                selectedMidiOut->startBackgroundThread();
        }
    }

    void ensureAudioDeviceRunning()
    {
        if (auto* audioDevice = audioDeviceManager->getCurrentAudioDevice()) {
            if (audioDevice->isPlaying())
                return;
        }
        // Reinitialize audio device to start it
        auto setup = audioDeviceManager->getAudioDeviceSetup();
        audioDeviceManager->setAudioDeviceSetup (setup, true);
    }

    // ValueTree::Listener - handle clock property changes
    void valueTreePropertyChanged (juce::ValueTree& tree, const juce::Identifier& property) override
    {
        if (tree != deviceData)
            return;

        if (property == Device::clockBpmID) {
            clockEngine.setBpm (device.clockBpm());
        } else if (property == Device::clockEnabledID) {
            if (device.clockEnabled() && ! clockEngine.isRunning()) {
                ensureAudioDeviceRunning();
                owner.addMidiMessage (clockEngine.start());
            } else if (! device.clockEnabled() && clockEngine.isRunning()) {
                owner.addMidiMessage (clockEngine.stop());
            }
        }
    }

    void valueTreeChildAdded (juce::ValueTree&, juce::ValueTree&) override {}
    void valueTreeChildRemoved (juce::ValueTree&, juce::ValueTree&, int) override {}
    void valueTreeChildOrderChanged (juce::ValueTree&, int, int) override {}
    void valueTreeParentChanged (juce::ValueTree&) override {}
    void valueTreeRedirected (juce::ValueTree&) override {}

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
    if (impl->virtualMidiOut != nullptr)
        impl->virtualMidiOut->stopBackgroundThread();
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

void Controller::updateMidiOutput() { impl->updateDefaultMidiOutput(); }

void Controller::addMidiMessage (const MidiMessage msg)
{
    // std::cout << msg.getDescription() << std::endl;

    if (impl->virtualMidiOut)
        impl->virtualMidiOut->sendMessageNow (msg);
    if (impl->selectedMidiOut)
        impl->selectedMidiOut->sendMessageNow (msg);
}

void Controller::audioDeviceIOCallbackWithContext (const float* const* inputChannelData,
                                                   int numInputChannels,
                                                   float* const* outputChannelData,
                                                   int numOutputChannels,
                                                   int numSamples,
                                                   const AudioIODeviceCallbackContext& context)
{
    juce::ignoreUnused (inputChannelData, numInputChannels, outputChannelData, numOutputChannels, context);

    // Generate MIDI clock messages with sample-accurate timing
    juce::MidiBuffer clockBuffer;
    impl->clockEngine.processBlock (clockBuffer, numSamples);

    // Send clock messages to MIDI outputs
    for (const auto metadata : clockBuffer) {
        addMidiMessage (metadata.getMessage());
    }
}

void Controller::audioDeviceAboutToStart (AudioIODevice* device)
{
    if (device != nullptr) {
        impl->clockEngine.prepare (device->getCurrentSampleRate());
    }
}

void Controller::audioDeviceStopped()
{
    impl->clockEngine.reset();
}
void Controller::audioDeviceError (const String& errorMessage) { juce::ignoreUnused (errorMessage); }

void Controller::addListener (Listener* listener) { impl->listeners.add (listener); }
void Controller::removeListener (Listener* listener) { impl->listeners.remove (listener); }

} // namespace vmc
