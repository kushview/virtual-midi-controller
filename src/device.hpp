/*
    This file is part of Virtual MIDI Controller
    Copyright (c) 2019  Kushview, LLC.  All rights reserved.
*/

#pragma once

#include "juce.hpp"

namespace vmc {

/** A virtual MIDI device. */
class Device : public ReferenceCountedObject {
public:
    using Ptr = ReferenceCountedObjectPtr<Device>;

    static const juce::Identifier nameID;
    static const juce::Identifier midiChannelID;
    static const juce::Identifier midiProgramID;

    /** Creates a new device. */
    Device();
    /** Destructor. */
    virtual ~Device();

    /** Returns the name of the device. */
    juce::String name() const noexcept { return _data.getProperty (nameID, juce::String()).toString(); }
    /** Returns the MIDI channel of the device. */
    int midiChannel() const noexcept { return _data.getProperty (midiChannelID, 0); }
    /** Returns the MIDI program of the device. */
    int midiProgram() const noexcept { return _data.getProperty (midiProgramID, 0); }

    /** Returns the underlying ValueTree data for this device. */
    const auto& data() const noexcept { return _data; }
    /** Returns a property from the device's data as a Value object. */
    juce::Value propertyAsValue (const juce::Identifier& ID) { return _data.getPropertyAsValue (ID, _undo); }

private:
    juce::ValueTree _data { "Device" };
    juce::UndoManager* _undo { nullptr };
};

} // namespace vmc
