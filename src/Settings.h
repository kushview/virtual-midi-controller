#pragma once

#include "JuceHeader.h"

namespace vmc {

class Settings :  public ApplicationProperties
{
public:
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
};

}
