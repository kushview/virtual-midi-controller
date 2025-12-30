/*
    This file is part of Virtual MIDI Controller
    Copyright (c) 2019  Kushview, LLC.  All rights reserved.
*/

#pragma once

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>

namespace vmc {

/** A virtual MIDI device. */
class Device final {
public:
    static const juce::Identifier nameID;
    static const juce::Identifier midiChannelID;
    static const juce::Identifier midiProgramID;
    static const juce::Identifier dialsID;
    static const juce::Identifier fadersID;

    /** Creates a new device. */
    Device();
    /** Destructor. */
    ~Device();

    /** Copy constructor. */
    Device (const Device& o)
        : _data (o._data), _undo (o._undo) {}
    /** Move constructor. */
    Device (Device&& o) noexcept
        : _data (std::move (o._data)), _undo (std::move (o._undo)) {}

    Device& operator= (const Device& o)
    {
        if (this != &o) {
            _data = o._data;
            _undo = o._undo;
        }
        return *this;
    }

    Device& operator= (Device&& o) noexcept
    {
        if (this != &o) {
            _data = std::move (o._data);
            _undo = std::move (o._undo);
        }
        return *this;
    }

    bool operator== (const Device& o) const noexcept
    {
        return _data == o._data;
    }

    bool operator!= (const Device& o) const noexcept
    {
        return ! (*this == o);
    }

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

    juce::ValueTree dials() const noexcept { return _data.getChildWithName (dialsID); }

    juce::ValueTree faders() const noexcept { return _data.getChildWithName (fadersID); }

    juce::String toXmlString() const { return _data.toXmlString(); }

private:
    juce::ValueTree _data { "Device" };
    juce::UndoManager* _undo { nullptr };
};

} // namespace vmc
