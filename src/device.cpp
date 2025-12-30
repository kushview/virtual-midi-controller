
#include "device.hpp"

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
        .setProperty (midiChannelID, 0, nullptr)
        .setProperty (midiProgramID, 0, nullptr);
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

} // namespace vmc
