// Copyright 2025 (c) Kushview, LLC
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

namespace vmc {

class Engine final {
public:
    Engine() = default;
    ~Engine() = default;

private:
    juce::AudioProcessorPlayer _player;
    Engine (const Engine&) = delete;
    Engine& operator= (const Engine&) = delete;
    Engine (Engine&&) = delete;
    Engine& operator= (Engine&&) = delete;
};

} // namespace vmc
