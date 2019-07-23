/*
    This file is part of Virtual MIDI Controller
    Copyright (c) 2019  Kushview, LLC.  All rights reserved.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

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
