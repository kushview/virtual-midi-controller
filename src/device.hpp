/*
    This file is part of Virtual MIDI Controller
    Copyright (c) 2019  Kushview, LLC.  All rights reserved.
*/

#pragma once

#include "juce.hpp"

namespace vmc {

class Device : public ReferenceCountedObject {
public:
    using Ptr = ReferenceCountedObjectPtr<Device>;
    Device() = default;
    virtual ~Device() = default;
};

} // namespace vmc
