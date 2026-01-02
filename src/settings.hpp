// Copyright 2025 (c) Kushview, LLC
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "juce.hpp"

namespace vmc {

class Settings : public ApplicationProperties {
public:
    static const char* lastMidiChannel;
    static const char* lastMidiProgram;
    static constexpr const char* dialMidiCCs = "dialMidiCCs";
    static constexpr const char* currentDrawer = "currentDrawer";

    Settings()
    {
        PropertiesFile::Options opts;
        opts.applicationName = "virtual-midi-controller";
        opts.filenameSuffix = "conf";
        opts.osxLibrarySubFolder = "Application Support";
        opts.storageFormat = PropertiesFile::storeAsCompressedBinary;

#if JUCE_DEBUG
        opts.applicationName << "_debug";
        opts.storageFormat = PropertiesFile::storeAsXML;
#endif

#if JUCE_LINUX
        opts.folderName = ".config/kushview/virtual-midi-controller";
#else
        opts.folderName = "Kushview/Virtual MIDI Controller";
#endif

        setStorageParameters (opts);
    }

    ~Settings() {}

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

    String getValue (const String& key, const String& defaultValue = "")
    {
        if (auto* props = getUserSettings())
            return props->getValue (key, defaultValue);
        return defaultValue;
    }
};

} // namespace vmc
