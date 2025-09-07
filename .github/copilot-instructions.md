# GitHub Copilot Instructions for Virtual MIDI Controller

This is a JUCE-based cross-platform MIDI controller application written in C++17. When working on this project, please keep these guidelines in mind:

## Project Structure
- `src/` - Main application source code
- `deps/` - Third-party dependencies (JUCE, libkv-auth, lua)
- `data/` - Application assets and resources
- `devices/` - MIDI device configurations
- `CMakeLists.txt` - CMake build configuration

## Key Technologies
- **JUCE Framework**: Used for GUI, audio, and MIDI functionality
- **Lua 5.3.5**: Embedded scripting for device configurations
- **CMake**: Build system (minimum version 3.22)
- **C++17**: Language standard

## Coding Conventions
- Follow JUCE naming conventions (CamelCase for classes, camelCase for methods)
- Use JUCE types when possible (String, Array, etc.)
- Prefer JUCE utilities over standard library where appropriate
- Keep Lua bindings in `bindings.cpp` and `bindings.hpp`

## Architecture Notes
- `MainComponent` is the primary UI class
- `Controller` manages MIDI I/O and device state
- `Settings` handles application configuration persistence
- `VirtualKeyboard` provides on-screen MIDI input
- Lua scripts define device-specific MIDI mappings

## Dependencies
For Ubuntu/Linux development, ensure these packages are installed:
```bash
sudo apt-get install build-essential cmake libcurl4-openssl-dev \
  libasound2-dev libjack-jackd2-dev libfreetype6-dev libx11-dev \
  libxext-dev libxrandr-dev libxinerama-dev libxcursor-dev \
  libgl1-mesa-dev libgtk-3-dev pkg-config
```

## Building
```bash
git submodule update --init --recursive
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel
```

## Testing
- Manual testing requires MIDI devices or virtual MIDI ports
- Focus on cross-platform compatibility (Linux, macOS, Windows)
- Test Lua script loading and execution
- Verify MIDI input/output functionality

## Common Tasks
- Adding new MIDI devices: Create Lua scripts in `devices/`
- UI modifications: Work in `MainComponent` and related files
- MIDI functionality: Extend `Controller` class
- New Lua bindings: Add to `bindings.cpp`

When suggesting code changes, prioritize JUCE best practices and maintain compatibility with the existing architecture.