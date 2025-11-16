cmake_minimum_required(VERSION 3.22)

project("Virtual MIDI Controller")
set(PROJECT_VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(deps/JUCE)
include_directories(deps/libkv-auth/include)

juce_add_gui_app(virtual-midi-controller
    COMPANY_NAME "Kushview"
    COMPANY_WEBSITE "https://kushview.net"
    PRODUCT_NAME "Virtual MIDI Controller"
    BUNDLE_ID "net.kushview.VirtualMIDIController")

target_sources(virtual-midi-controller 
    PRIVATE
        src/settings.cpp
        src/controller.cpp
        src/main.cpp
        src/maincomponent.cpp
        src/lookandfeel.cpp
        src/midicceditor.cpp
        src/virtualkeyboard.cpp
)

target_compile_definitions(virtual-midi-controller
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=1
        JUCE_LOAD_CURL_SYMBOLS_LAZILY=1
        JUCE_VST3_CAN_REPLACE_VST2=0)
target_include_directories(virtual-midi-controller
    PRIVATE 
        src)

juce_add_binary_data(VMCData
    SOURCES
        data/vmc-logo.png)

target_link_libraries(virtual-midi-controller PRIVATE 
    juce_core 
    juce_gui_basics 
    juce_audio_devices
    juce_audio_utils
    juce_product_unlocking
    VMCData)
