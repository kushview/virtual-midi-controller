// Copyright 2025 (c) Kushview, LLC
// SPDX-License-Identifier: GPL-3.0-or-later

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
    static const juce::Identifier RangedID;
    static const juce::Identifier ccNumberID;
    static const juce::Identifier valueID;

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
        _data = o._data;
        _undo = o._undo;
        return *this;
    }

    Device& operator= (Device&& o) noexcept
    {
        _data = std::move (o._data);
        _undo = std::move (o._undo);
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

    bool isValid() const noexcept { return _data.isValid(); }

    /** Returns the name of the device. */
    juce::String name() const noexcept { return _data.getProperty (nameID, juce::String()).toString(); }
    void setName (std::string_view newName) { _data.setProperty (nameID, newName.data(), _undo); }

    /** Returns the MIDI channel of the device. */
    int midiChannel() const noexcept { return _data.getProperty (midiChannelID, 0); }
    void setMidiChannel (int newChannel);

    /** Returns the MIDI program of the device. */
    int midiProgram() const noexcept { return _data.getProperty (midiProgramID, 0); }
    void setMidiProgram (int newProgram);

    /** Returns the underlying ValueTree data for this device. */
    const auto& data() const noexcept { return _data; }
    /** Returns a property from the device's data as a Value object. */
    juce::Value propertyAsValue (const juce::Identifier& ID) { return _data.getPropertyAsValue (ID, _undo); }

    juce::ValueTree dials() const noexcept { return _data.getChildWithName (dialsID); }

    juce::ValueTree faders() const noexcept { return _data.getChildWithName (fadersID); }

    juce::String toXmlString() const { return _data.toXmlString(); }

    void setUndoManager (juce::UndoManager* undo)
    {
        _undo = undo;
    }

    bool load (const juce::File&);
    void save (const juce::File&) const;

private:
    juce::ValueTree _data { "Device" };
    juce::UndoManager* _undo { nullptr };
};

} // namespace vmc
