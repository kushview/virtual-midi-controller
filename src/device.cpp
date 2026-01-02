// Copyright 2025 (c) Kushview, LLC
// SPDX-License-Identifier: GPL-3.0-or-later

#include "device.hpp"

using juce::ValueTree;

namespace vmc {
namespace detail {
static juce::ValueTree makeRanged()
{
    juce::ValueTree out { "Ranged" };
    out.setProperty ("ccNumber", 0, nullptr)
        .setProperty ("value", 0, nullptr);
    return out;
}
} // namespace detail

const juce::Identifier Device::nameID = "name";
const juce::Identifier Device::midiChannelID = "midiChannel";
const juce::Identifier Device::midiProgramID = "midiProgram";
const juce::Identifier Device::dialsID = "dials";
const juce::Identifier Device::fadersID = "faders";

Device::Device()
{
    _data.setProperty (nameID, "VMC", nullptr)
        .setProperty (midiChannelID, 1, nullptr)
        .setProperty (midiProgramID, 1, nullptr);
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

} // namespace vmc
