// Copyright 2025 (c) Kushview, LLC
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_data_structures/juce_data_structures.h>

namespace vmc {

class Controller;
class MainComponent;

// Custom cell widget for MIDI CC number editing
class CCNumberEditor : public juce::Component {
public:
    CCNumberEditor();
    ~CCNumberEditor() override;

    void setValue (int ccNumber);
    int getValue() const;

    /** Puts the cell into a non-editable state, optionally showing custom text
        (e.g. "PB" for pitch bend controls that have no CC number). */
    void setReadOnly (bool readOnly, const juce::String& displayText = {});

    std::function<void (int)> onValueChanged;

    void resized() override;
    void focusLost (juce::Component::FocusChangeType cause) override;

private:
    juce::TextEditor textEditor;
    int currentValue = 0;

    void validateAndUpdate();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CCNumberEditor)
};

// Custom cell widget for control name editing using an editable label
class ControlNameEditor : public juce::Component,
                          public juce::Label::Listener {
public:
    ControlNameEditor();
    ~ControlNameEditor() override;

    void setText (const juce::String& text);
    juce::String getText() const;

    std::function<void (const juce::String&)> onTextChanged;

    void resized() override;

    // Label::Listener override
    void labelTextChanged (juce::Label* labelThatHasChanged) override;

private:
    juce::Label nameLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ControlNameEditor)
};

// Structure to hold mapping data for each UI component
struct MidiCCMapping {
    juce::String componentName;
    juce::Component* component = nullptr;
    juce::ValueTree valueTree; // the control's Ranged child in the Device data
    int ccNumber = -1;         // -1 means no mapping
    int midiChannel = 1;
    bool isLearning = false;
    bool ccEditable = true; // false for controls without a CC (e.g. pitch bend)

    enum ComponentType {
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
                     public juce::TableListBoxModel {
public:
    MidiCCEditor (Controller& controller);
    ~MidiCCEditor() override;

    // Component overrides
    void paint (juce::Graphics& g) override;
    void resized() override;

    // TableListBoxModel overrides
    int getNumRows() override;
    void paintRowBackground (juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
    void paintCell (juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
    juce::Component* refreshComponentForCell (int rowNumber, int columnId, bool isRowSelected, juce::Component* existingComponentToUpdate) override;

    // Table setup
    void setupTable();
    void refreshMappings();
    void addMapping (const juce::String& name, juce::Component* comp, MidiCCMapping::ComponentType type,
                     juce::ValueTree valueTree = {}, bool ccEditable = true);

    // MIDI CC functionality
    void setCCMapping (int row, int ccNumber);
    void setControlName (int row, const juce::String& name);

    // Toggle drawer visibility
    void setVisible (bool shouldBeVisible) override;
    bool isDrawerOpen() const { return drawerOpen; }
    void toggleDrawer();

private:
    Controller& controller;
    juce::TableListBox table;
    juce::Array<MidiCCMapping> mappings;

    bool drawerOpen = false;

    // Column IDs
    enum ColumnIds {
        ControlNameColumn = 1,
        CCNumberColumn = 2
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiCCEditor)
};

// Drawer container that handles instant show/hide
class MidiCCDrawer : public juce::Component {
public:
    MidiCCDrawer (Controller& controller);
    ~MidiCCDrawer() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

    void toggleDrawer();
    bool isOpen() const { return isDrawerOpen; }

    void setDrawerHeight (int height) { targetDrawerHeight = height; }
    int getDrawerHeight() const { return targetDrawerHeight; }

    MidiCCEditor& getEditor() { return editor; }

    // Callback for when drawer size changes
    std::function<void (int newHeight)> onSizeChanged;

private:
    Controller& controller;
    MidiCCEditor editor;

    bool isDrawerOpen = false;
    int currentDrawerHeight = 0;
    int targetDrawerHeight = 200;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiCCDrawer)
};

} // namespace vmc
