
#include "Controller.h"

namespace vmc {

struct Controller::Impl : public MidiKeyboardStateListener
{
    Impl() { }
    ~Impl() { }

    Settings settings;
    KnownPluginList knownPlugins;
    OptionalScopedPointer<AudioDeviceManager> devices;
    OptionalScopedPointer<AudioFormatManager> formats;
    OptionalScopedPointer<AudioPluginFormatManager> plugins;
    std::unique_ptr<MidiOutput> midiOut;
    MidiKeyboardState keyboardState;

    void handleNoteOn (MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity) override
    {
        midiOut->sendMessageNow (MidiMessage::noteOn (midiChannel, midiNoteNumber, velocity));
    }

    void handleNoteOff (MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity) override
    {
        midiOut->sendMessageNow (MidiMessage::noteOff (midiChannel, midiNoteNumber, velocity));
    }
};

Controller::Controller()
{
    impl.reset (new Impl());
    impl->devices.setOwned (new AudioDeviceManager ());
    impl->formats.setOwned (new AudioFormatManager ());
    impl->plugins.setOwned (new AudioPluginFormatManager());
    impl->midiOut.reset (MidiOutput::createNewDevice ("VMC"));
    impl->midiOut->startBackgroundThread();
    impl->keyboardState.addListener (impl.get());
}

Controller::~Controller()
{
    impl->keyboardState.removeListener (impl.get());

    impl->midiOut->stopBackgroundThread();
    impl->formats->clearFormats();
    impl.reset();
}

void Controller::initializeAudioDevice()
{
    auto& devices = getDeviceManager();
    auto& settings = impl->settings;
    bool initDefault = true;
    
    if (auto* const props = settings.getUserSettings())
    {
        if (auto* xml = props->getXmlValue ("devices"))
        {
            initDefault = devices.initialise (32, 32, xml, false).isNotEmpty();
            deleteAndZero (xml);
        }
    }
    
    if (initDefault)
    {
        devices.initialiseWithDefaultDevices (32, 32);
    }

    devices.addAudioCallback (this);
    devices.addMidiInputCallback (String(), this);
}

void Controller::initializePlugins()
{
    auto& settings = impl->settings;
    auto& plugins = getPluginManager();
    plugins.addDefaultFormats();
    if (auto* const props = settings.getUserSettings())
    {
        const auto file = props->getFile().getParentDirectory().getChildFile("plugins.xml");
        if (auto* xml = XmlDocument::parse (file))
        {
            impl->knownPlugins.recreateFromXml (*xml);
            deleteAndZero (xml);
        }
    }
}

void Controller::shutdown()
{
    auto& devices = getDeviceManager();
    devices.removeAudioCallback (this);
    devices.removeMidiInputCallback (String(), this);
    devices.closeAudioDevice();
}

void Controller::saveSettings()
{
    auto& settings = impl->settings;
    auto& devices  = getDeviceManager();
    auto& plugins  = getPluginManager();
    auto& formats  = getAudioFormats();

    if (auto* const props = settings.getUserSettings())
    {
        if (auto* devicesXml = devices.createStateXml())
        {
            props->setValue ("devices", devicesXml);
            deleteAndZero (devicesXml);
        }

        if (auto* knownPlugins = impl->knownPlugins.createXml())
        {
            const auto file = props->getFile().getParentDirectory().getChildFile("plugins.xml");
            knownPlugins->writeToFile (file, String());
            deleteAndZero (knownPlugins);
        }
    }
}

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

MidiKeyboardState& Controller::getMidiKeyboardState()         { return impl->keyboardState; }
Settings& Controller::getSettings()                           { return impl->settings; }
AudioDeviceManager& Controller::getDeviceManager()            { return *impl->devices; }
AudioFormatManager& Controller::getAudioFormats()             { return *impl->formats; }
AudioPluginFormatManager& Controller::getPluginManager()      { return *impl->plugins; }

void Controller::addMidiMessage (const MidiMessage msg)
{
    impl->midiOut->sendMessageNow (msg);
}

void Controller::audioDeviceIOCallback (const float** inputChannelData,
                                      int numInputChannels, float** outputChannelData,
                                      int numOutputChannels, int numSamples) {}
void Controller::audioDeviceAboutToStart (AudioIODevice* device) {}
void Controller::audioDeviceStopped() { }
void Controller::audioDeviceError (const String& errorMessage) { }

}
