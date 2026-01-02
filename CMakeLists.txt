cmake_minimum_required(VERSION 3.22)

project("virtual-midi-controller" VERSION 1.0.0)

set(VMC_DESCRIPTION_SUMMARY "A cross-platform MIDI controller application with virtual keyboard")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_OSX_ARCHITECTURES arm64;x86_64)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13")

add_subdirectory(deps/JUCE EXCLUDE_FROM_ALL)

juce_add_gui_app(virtual-midi-controller
    PRODUCT_NAME "Virtual MIDI Controller"
    COMPANY_NAME "Kushview"
    COMPANY_WEBSITE "https://kushview.net"
    BUNDLE_ID "net.kushview.VirtualMIDIController"
    ICON_BIG "data/icon.png"
    ICON_SMALL "data/icon.png"
)

if(LINUX)
    # Linux conventions want all lowercase executable names
    set_target_properties(virtual-midi-controller PROPERTIES
        OUTPUT_NAME "virtual-midi-controller"
    )
endif()

target_sources(virtual-midi-controller 
    PRIVATE
        src/settings.cpp
        src/controller.cpp
        src/device.cpp
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
        JUCE_VST3_CAN_REPLACE_VST2=0
        VMC_VERSION_STRING=\"${PROJECT_VERSION}\")
target_include_directories(virtual-midi-controller
    PRIVATE 
        src)

juce_add_binary_data(vmcdata
    SOURCES
        data/vmc-logo.png)

target_link_libraries(virtual-midi-controller PRIVATE 
    juce_core 
    juce_gui_basics 
    juce_audio_devices
    juce_audio_utils
    vmcdata)

include(GNUInstallDirs)

if(LINUX)
    # Configure and install desktop file
    configure_file(
        "${PROJECT_SOURCE_DIR}/data/virtual-midi-controller.desktop.in"
        "${PROJECT_BINARY_DIR}/virtual-midi-controller.desktop"
        @ONLY)
    
    install(FILES "${PROJECT_BINARY_DIR}/virtual-midi-controller.desktop"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications")
    
    # Install icon for desktop file
    install(FILES "${PROJECT_SOURCE_DIR}/data/icon.png"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/512x512/apps"
        RENAME "virtual-midi-controller.png")
    
    install(TARGETS virtual-midi-controller 
        DESTINATION "${CMAKE_INSTALL_BINDIR}")
elseif(APPLE)
    install(TARGETS virtual-midi-controller 
        BUNDLE DESTINATION "${CMAKE_INSTALL_PREFIX}" 
        COMPONENT Standalone)
elseif(WIN32)
    install(TARGETS virtual-midi-controller 
        DESTINATION "${CMAKE_INSTALL_PREFIX}")
endif()

set(CPACK_PACKAGE_NAME "Virtual MIDI Controller")
set(CPACK_PACKAGE_VENDOR "Kushview")
set(CPACK_PACKAGE_CONTACT "Kushview Support <support@kushview.net>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${VMC_DESCRIPTION_SUMMARY}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "VMC")
set(CPACK_RESOURCE_FILE_README "${PROJECT_SOURCE_DIR}/LICENSE.txt")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE.txt")
set(CPACK_COMPONENT_INSTALL OFF)

if(LINUX)
    set(VMC_SYSTEM_NAME "linux")
    set(CPACK_PACKAGE_NAME "virtual-midi-controller")
    string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" VMC_PROCESSOR)
    set(CPACK_GENERATOR "DEB")
    set(CPACK_PACKAGING_INSTALL_PREFIX "/usr")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libasound2;libfreetype6;libx11-6;libxext6;libxrandr2;libxinerama1;libxcursor1;libcurl4;libgl1;libstdc++6")
elseif(APPLE)
    set(VMC_SYSTEM_NAME "macos")
    set(VMC_PROCESSOR "universal")
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_DMG_VOLUME_NAME "Virtual MIDI Controller")
    set(CPACK_DMG_BACKGROUND_IMAGE "${PROJECT_SOURCE_DIR}/data/dmg.png")
    set(CPACK_DMG_DS_STORE_SETUP_SCRIPT "${PROJECT_SOURCE_DIR}/data/dmg-setup.applescript")
    set(CPACK_DMG_FORMAT "UDBZ")
    set(CPACK_SET_DESTDIR ON)
    set(CPACK_PACKAGING_INSTALL_PREFIX "/")
elseif(WIN32)
    set(VMC_SYSTEM_NAME "windows")
    string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" VMC_PROCESSOR)
    set(CPACK_GENERATOR "ZIP")
    set(CPACK_PACKAGING_INSTALL_PREFIX "/")
endif()

set(VMC_GENERATOR ${CPACK_GENERATOR})
set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${CPACK_PACKAGE_VERSION}-${VMC_SYSTEM_NAME}-${VMC_PROCESSOR}")
include(CPack)

# Clean CPack staging directory before and after packaging to prevent macOS relocation issues
add_custom_target(installer
    COMMAND ${CMAKE_COMMAND} -E rm -rf "${PROJECT_BINARY_DIR}/_CPack_Packages"
    COMMAND ${CMAKE_COMMAND} --build "${PROJECT_BINARY_DIR}" --target package
    COMMAND ${CMAKE_COMMAND} -E rm -rf "${PROJECT_BINARY_DIR}/_CPack_Packages"
    COMMENT "Cleaning CPack staging and building package"
    VERBATIM
    USES_TERMINAL)
