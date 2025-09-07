# Virtual MIDI Controller
![Screenshot](data/screenshot.png)

A software MIDI controller which can send MIDI to any input device.  Also exposes itself as a MIDI input to other applications (OSX only)

## Building with CMake

This project uses CMake as its build system. You'll need CMake 3.22 or later.

### Prerequisites for Linux

Install all required dependencies for JUCE development:

```bash
sudo apt-get update
sudo apt-get install -y \
  build-essential \
  cmake \
  libcurl4-openssl-dev \
  libasound2-dev \
  libjack-jackd2-dev \
  libfreetype6-dev \
  libx11-dev \
  libxext-dev \
  libxrandr-dev \
  libxinerama-dev \
  libxcursor-dev \
  libgl1-mesa-dev \
  libgtk-3-dev \
  pkg-config
```

### Build Steps

1. Initialize submodules:
   ```bash
   git submodule update --init --recursive
   ```
   
   **Note**: If the `libkv-auth` submodule fails to download (private repository), the build system will use stub implementations that disable unlock functionality but allow the project to build successfully.

2. Create a build directory and configure:
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   ```

3. Build the project:
   ```bash
   cmake --build . --config Release --parallel
   ```

4. The executable will be created at:
   ```
   build/virtual-midi-controller_artefacts/Release/Virtual MIDI Controller
   ```

### Automated Builds

GitHub Actions automatically builds the project for Linux on every push and pull request. The built artifacts are available for download from the Actions tab.

## VSCode Support

There are launch and build tasks in the `.vscode` folder, though these may need updating to use CMake instead of the previous build system.
