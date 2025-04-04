cmake_minimum_required(VERSION 3.22)

project("Virtual MIDI Controller")
set(PROJECT_VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(deps/JUCE)

juce_add_gui_app(VMC
    PRODUCT_NAME "Virtual MIDI Controller")

target_sources(VMC 
    PRIVATE
        src/lualib.cpp
        src/settings.cpp
        src/controller.cpp
        src/bindings.cpp
        src/main.cpp
        src/maincomponent.cpp
)

target_compile_definitions(VMC
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)
target_include_directories(VMC PRIVATE 
    src
    deps/lua-5.3.5/src)

target_link_libraries(VMC PRIVATE 
    juce_core 
    juce_gui_basics 
    juce_audio_devices
    juce_audio_utils)
