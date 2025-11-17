
#include "device.hpp"

namespace vmc {

const juce::Identifier Device::nameID = "name";
const juce::Identifier Device::midiChannelID = "midiChannel";
const juce::Identifier Device::midiProgramID = "midiProgram";

Device::Device()
{
    _data.setProperty (nameID, "VMC", nullptr)
        .setProperty (midiChannelID, 0, nullptr)
        .setProperty (midiProgramID, 0, nullptr);
}

Device::~Device()
{
    _undo = nullptr;
}

} // namespace vmc
