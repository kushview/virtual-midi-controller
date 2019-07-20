#pragma once

#include "JuceHeader.h"

namespace vmc {

class Settings :  public ApplicationProperties
{
public:
    static const char* lastMidiChannel;
    static const char* lastMidiProgram;

    Settings()
    {
        PropertiesFile::Options opts;
        opts.applicationName     = "VirtualMidiController";
        opts.filenameSuffix      = "conf";
        opts.osxLibrarySubFolder = "Application Support";
        opts.storageFormat       = PropertiesFile::storeAsCompressedBinary;

       #if JUCE_DEBUG
        opts.applicationName << "Debug";
        opts.storageFormat       = PropertiesFile::storeAsXML;
       #endif
        
       #if JUCE_LINUX
        opts.folderName          = ".config/virtual-midi-controller";
       #else
        opts.folderName          = "VirtualMidiController";
       #endif

        setStorageParameters (opts);
    }

    ~Settings() { }

    void set (const String& key, const var& value)
    {
        if (auto* const props = getUserSettings())
            props->setValue (key, value);
    }

    int getInt (const String& key, int defaultValue = 0)
    {
        if (auto* props = getUserSettings())
            return props->getIntValue (key, defaultValue);
        return defaultValue;
    }
};

}
