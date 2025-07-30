/*
    This file is part of Virtual MIDI Controller
    Copyright (c) 2019  Kushview, LLC.  All rights reserved.
*/

#pragma once

#include "juce.hpp"
#include <juce_gui_basics/juce_gui_basics.h>

namespace vmc {

class Controller;
class MainComponent;

// Custom cell widget for MIDI CC number editing
class CCNumberEditor : public juce::Component
{
public:
    CCNumberEditor();
    ~CCNumberEditor() override;
    
    void setValue(int ccNumber);
    int getValue() const;
    
    std::function<void(int)> onValueChanged;
    
    void resized() override;
    void focusLost(juce::Component::FocusChangeType cause) override;
    
private:
    juce::TextEditor textEditor;
    int currentValue = 0;
    
    void validateAndUpdate();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CCNumberEditor)
};

// Custom cell widget for MIDI channel editing
class ChannelEditor : public juce::Component
{
public:
    ChannelEditor();
    ~ChannelEditor() override;
    
    void setValue(int channel);
    int getValue() const;
    
    std::function<void(int)> onValueChanged;
    
    void resized() override;
    
private:
    juce::ComboBox comboBox;
    int currentValue = 1;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChannelEditor)
};

// Custom cell widget for MIDI learn button
class LearnButton : public juce::Component
{
public:
    LearnButton();
    ~LearnButton() override;
    
    void setLearning(bool isLearning);
    bool isLearning() const { return learning; }
    
    std::function<void()> onLearnClicked;
    
    void resized() override;
    
private:
    juce::TextButton button;
    bool learning = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LearnButton)
};

// Structure to hold mapping data for each UI component
struct MidiCCMapping
{
    juce::String componentName;
    juce::Component* component = nullptr;
    int ccNumber = -1;  // -1 means no mapping
    int midiChannel = 1;
    bool isLearning = false;
    
    enum ComponentType
    {
        VerticalSlider,
        Dial,
        ProgramSlider,
        ChannelSlider,
        Unknown
    };
    
    ComponentType type = Unknown;
};

// Main MIDI CC Editor table component
class MidiCCEditor : public juce::Component,
                     public juce::TableListBoxModel
{
public:
    MidiCCEditor(Controller& controller);
    ~MidiCCEditor() override;
    
    // Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // TableListBoxModel overrides
    int getNumRows() override;
    void paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
    void paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
    juce::Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, juce::Component* existingComponentToUpdate) override;
    
    // Table setup
    void setupTable();
    void refreshMappings();
    void addMapping(const juce::String& name, juce::Component* comp, MidiCCMapping::ComponentType type);
    
    // MIDI CC functionality
    void setCCMapping(int row, int ccNumber);
    void setChannelMapping(int row, int channel);
    void startLearning(int row);
    void stopLearning();
    
    // Toggle drawer visibility
    void setVisible(bool shouldBeVisible) override;
    bool isDrawerOpen() const { return drawerOpen; }
    void toggleDrawer();
    
private:
    Controller& controller;
    juce::TableListBox table;
    juce::Array<MidiCCMapping> mappings;
    
    bool drawerOpen = false;
    int learningRow = -1;
    
    // Column IDs
    enum ColumnIds
    {
        ComponentNameColumn = 1,
        CCNumberColumn = 2,
        ChannelColumn = 3,
        LearnColumn = 4
    };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiCCEditor)
};

// Drawer container that handles instant show/hide
class MidiCCDrawer : public juce::Component
{
public:
    MidiCCDrawer(Controller& controller);
    ~MidiCCDrawer() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void toggleDrawer();
    bool isOpen() const { return isDrawerOpen; }
    
    void setDrawerHeight(int height) { targetDrawerHeight = height; }
    int getDrawerHeight() const { return targetDrawerHeight; }
    
    MidiCCEditor& getEditor() { return editor; }
    
    // Callback for when drawer size changes
    std::function<void(int newHeight)> onSizeChanged;
    
private:
    Controller& controller;
    MidiCCEditor editor;
    
    bool isDrawerOpen = false;
    int currentDrawerHeight = 0;
    int targetDrawerHeight = 200;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiCCDrawer)
};

}
