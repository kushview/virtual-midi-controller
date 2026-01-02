// Copyright 2025 (c) Kushview, LLC
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

namespace vmc {

class Engine final : public AudioIODeviceCallback,
                     public MidiInputCallback {
public:
    Engine() = default;
    ~Engine() = default;

    Engine (const Engine&) = delete;
    Engine& operator= (const Engine&) = delete;
    Engine (Engine&&) = delete;
    Engine& operator= (Engine&&) = delete;
};

} // namespace vmc
