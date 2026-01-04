// Copyright 2025 (c) Kushview, LLC
// SPDX-License-Identifier: GPL-3.0-or-later

#include "device.hpp"

using juce::ValueTree;

namespace vmc {
namespace detail {
static juce::ValueTree makeRanged()
{
    juce::ValueTree out { Device::RangedID };
    out.setProperty (Device::ccNumberID, 0, nullptr)
        .setProperty (Device::valueID, 0, nullptr);
    return out;
}
} // namespace detail

const juce::Identifier Device::nameID = "name";
const juce::Identifier Device::midiChannelID = "midiChannel";
const juce::Identifier Device::midiProgramID = "midiProgram";
const juce::Identifier Device::dialsID = "dials";
const juce::Identifier Device::fadersID = "faders";
const juce::Identifier Device::RangedID = "Ranged";
const juce::Identifier Device::ccNumberID = "ccNumber";
const juce::Identifier Device::valueID = "value";
const juce::Identifier Device::clockBpmID = "clockBpm";
const juce::Identifier Device::clockEnabledID = "clockEnabled";

Device::Device()
{
    _data.setProperty (nameID, "VMC", nullptr)
        .setProperty (midiChannelID, 1, nullptr)
        .setProperty (midiProgramID, 1, nullptr)
        .setProperty (clockBpmID, 120.0, nullptr)
        .setProperty (clockEnabledID, false, nullptr);
    auto things = _data.getOrCreateChildWithName (dialsID, nullptr);
    for (int i = 0; i < 8; ++i)
        things.appendChild (detail::makeRanged(), nullptr);
    things = _data.getOrCreateChildWithName (fadersID, nullptr);
    for (int i = 0; i < 2; ++i)
        things.appendChild (detail::makeRanged(), nullptr);
}

Device::~Device()
{
    _undo = nullptr;
}

bool Device::load (const juce::File& xml)
{
    ValueTree newData;
    if (auto xmlElement = juce::XmlDocument::parse (xml))
        newData = juce::ValueTree::fromXml (*xmlElement);
    if (newData.isValid()) {
        _data.removeAllProperties (nullptr);
        _data.removeAllChildren (nullptr);
        _data.copyPropertiesAndChildrenFrom (newData, nullptr);
        return true;
    }
    return false;
}

void Device::save (const juce::File& file) const
{
    if (auto xml = _data.createXml())
        xml->writeTo (file);
}

void Device::setMidiChannel (int newChannel)
{
    _data.setProperty (midiChannelID, juce::jlimit (1, 16, newChannel), _undo);
}

void Device::setMidiProgram (int newProgram)
{
    _data.setProperty (midiProgramID, juce::jlimit (1, 128, newProgram), _undo);
}

void Device::setClockBpm (double bpm)
{
    _data.setProperty (clockBpmID, juce::jlimit (20.0, 900.0, bpm), _undo);
}

void Device::setClockEnabled (bool enabled)
{
    _data.setProperty (clockEnabledID, enabled, _undo);
}

} // namespace vmc
