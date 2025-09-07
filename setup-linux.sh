#!/bin/bash

# Setup script for Virtual MIDI Controller Linux development environment
# This script installs dependencies and prepares the build environment

set -e

echo "Setting up Virtual MIDI Controller build environment for Linux..."

# Install JUCE dependencies
echo "Installing system dependencies..."
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

echo "Initializing git submodules..."
git submodule update --init --recursive

# Check if JUCE is available
if [ ! -f deps/JUCE/CMakeLists.txt ]; then
    echo "JUCE submodule failed, downloading manually..."
    rm -rf deps/JUCE
    git clone https://github.com/juce-framework/JUCE.git deps/JUCE
    echo "JUCE downloaded successfully"
fi

# Check if libkv-auth is available
if [ ! -d deps/libkv-auth/include ]; then
    echo "libkv-auth not available (private repository), using stubs..."
    echo "The project will build successfully but without unlock functionality."
fi

echo "Environment setup complete!"
echo ""
echo "To build the project:"
echo "  mkdir build && cd build"
echo "  cmake .. -DCMAKE_BUILD_TYPE=Release"
echo "  cmake --build . --parallel"
echo ""