/*
    This file is part of Virtual MIDI Controller
    Copyright (c) 2019  Kushview, LLC.  All rights reserved.
*/

#include "midicceditor.hpp"
#include "controller.hpp"

namespace vmc {

//==============================================================================
// CCNumberEditor Implementation
//==============================================================================

CCNumberEditor::CCNumberEditor()
{
    addAndMakeVisible(textEditor);
    textEditor.setInputRestrictions(3, "0123456789");
    textEditor.setText("0", juce::dontSendNotification);
    textEditor.setJustification(juce::Justification::centred);
    
    // Style to match aluminum theme
    textEditor.setColour(juce::TextEditor::backgroundColourId, juce::Colour::fromRGB(35, 38, 42));
    textEditor.setColour(juce::TextEditor::textColourId, juce::Colours::white.withAlpha(0.9f));
    textEditor.setColour(juce::TextEditor::outlineColourId, juce::Colours::white.withAlpha(0.2f));
    textEditor.setColour(juce::TextEditor::focusedOutlineColourId, juce::Colours::white.withAlpha(0.4f));
    textEditor.setFont(juce::Font(11.0f));
    
    textEditor.onReturnKey = [this]() { validateAndUpdate(); };
    textEditor.onEscapeKey = [this]() { 
        textEditor.setText(juce::String(currentValue), juce::dontSendNotification);
        unfocusAllComponents();
    };
    textEditor.onFocusLost = [this]() { validateAndUpdate(); };
}

CCNumberEditor::~CCNumberEditor() = default;

void CCNumberEditor::setValue(int ccNumber)
{
    currentValue = juce::jlimit(0, 127, ccNumber);
    textEditor.setText(juce::String(currentValue), juce::dontSendNotification);
}

int CCNumberEditor::getValue() const
{
    return currentValue;
}

void CCNumberEditor::resized()
{
    textEditor.setBounds(getLocalBounds().reduced(2));
}

void CCNumberEditor::focusLost(juce::Component::FocusChangeType cause)
{
    validateAndUpdate();
}

void CCNumberEditor::validateAndUpdate()
{
    int newValue = textEditor.getText().getIntValue();
    newValue = juce::jlimit(0, 127, newValue);
    
    if (newValue != currentValue)
    {
        currentValue = newValue;
        textEditor.setText(juce::String(currentValue), juce::dontSendNotification);
        
        if (onValueChanged)
            onValueChanged(currentValue);
    }
}

//==============================================================================
// MidiCCEditor Implementation
//==============================================================================

MidiCCEditor::MidiCCEditor(Controller& ctrl) : controller(ctrl)
{
    addAndMakeVisible(table);
    table.setModel(this);
    setupTable();
}

MidiCCEditor::~MidiCCEditor() = default;

void MidiCCEditor::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    // Match the main UI aluminum styling
    g.setColour(juce::Colour::fromRGB(45, 48, 52));
    g.fillRect(bounds);

    // Subtle radial gradient for depth (lighter center)
    juce::ColourGradient grad(
        juce::Colour::fromRGB(52, 55, 58), getWidth() * 0.5f, getHeight() * 0.5f,
        juce::Colour::fromRGB(42, 45, 48), 0.0f, 0.0f,
        true);
    g.setGradientFill(grad);
    g.fillRect(bounds);

    // Draw horizontal lines of the mesh (lighter pattern for table)
    for (int i = 0; i < getHeight(); i += 4)
    {
        g.setColour(juce::Colours::white.withAlpha(0.015f));
        g.drawHorizontalLine(i, 0, (float)getWidth());
    }

    // Draw vertical lines of the mesh (lighter pattern for table)
    for (int i = 0; i < getWidth(); i += 4)
    {
        g.setColour(juce::Colours::white.withAlpha(0.01f));
        g.drawVerticalLine(i, 0, (float)getHeight());
    }
    
    // Header area with slightly different color
    auto headerBounds = bounds.removeFromTop(25);
    g.setColour(juce::Colour::fromRGB(40, 43, 47));
    g.fillRect(headerBounds);
    
    // Header text
    g.setColour(juce::Colours::white.withAlpha(0.9f));
    g.setFont(juce::Font(juce::FontOptions (14.0f, juce::Font::bold)));
    g.drawText("MIDI CC Mappings", headerBounds, juce::Justification::centred);
    
    // Subtle outline that blends with the brush effect
    g.setColour(juce::Colours::black.withAlpha(0.15f));
    g.drawRect(getLocalBounds());
}

void MidiCCEditor::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop(25); // Space for header
    table.setBounds(bounds);
}

void MidiCCEditor::setupTable()
{
    table.getHeader().addColumn("Component", ComponentNameColumn, 200);
    table.getHeader().addColumn("CC#", CCNumberColumn, 100);
    
    table.setHeaderHeight(22);
    table.setRowHeight(24);
    table.setColour(juce::ListBox::backgroundColourId, juce::Colours::transparentBlack);
    table.setColour(juce::ListBox::outlineColourId, juce::Colours::white.withAlpha(0.1f));
    
    // Style the header to match the aluminum theme
    table.getHeader().setColour(juce::TableHeaderComponent::backgroundColourId, 
                               juce::Colour::fromRGB(40, 43, 47));
    table.getHeader().setColour(juce::TableHeaderComponent::textColourId, 
                               juce::Colours::white.withAlpha(0.8f));
    table.getHeader().setColour(juce::TableHeaderComponent::outlineColourId, 
                               juce::Colours::white.withAlpha(0.15f));
}

void MidiCCEditor::refreshMappings()
{
    mappings.clear();
    // This will be called from MainComponent to populate the mappings
    table.updateContent();
}

void MidiCCEditor::addMapping(const juce::String& name, juce::Component* comp, MidiCCMapping::ComponentType type)
{
    MidiCCMapping mapping;
    mapping.componentName = name;
    mapping.component = comp;
    mapping.type = type;
    mapping.midiChannel = 1; // Default channel
    
    // Set default CC numbers based on current hardcoded values
    switch (type)
    {
        case MidiCCMapping::VerticalSlider:
            if (mappings.size() == 0) mapping.ccNumber = 60;
            else if (mappings.size() == 1) mapping.ccNumber = 61;
            else if (mappings.size() == 2) mapping.ccNumber = 62;
            break;
        case MidiCCMapping::Dial:
            mapping.ccNumber = 102 + (mappings.size() - 3); // Assuming 3 sliders come first
            break;
        default:
            mapping.ccNumber = -1; // No default mapping
            break;
    }
    
    mappings.add(mapping);
    table.updateContent();
}

int MidiCCEditor::getNumRows()
{
    return mappings.size();
}

void MidiCCEditor::paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
    {
        // Selected row with aluminum tint
        g.setColour(juce::Colour::fromRGB(65, 68, 72).withAlpha(0.8f));
        g.fillRect(0, 0, width, height);
    }
    else if (rowNumber % 2 == 0)
    {
        // Alternate rows with subtle aluminum variation
        g.setColour(juce::Colour::fromRGB(48, 51, 55).withAlpha(0.3f));
        g.fillRect(0, 0, width, height);
    }
    else
    {
        // Default transparent for odd rows
        g.setColour(juce::Colours::transparentBlack);
        g.fillRect(0, 0, width, height);
    }
    
    // Subtle row separator line
    g.setColour(juce::Colours::white.withAlpha(0.05f));
    g.drawHorizontalLine(height - 1, 0, (float)width);
}

void MidiCCEditor::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    if (rowNumber >= mappings.size())
        return;
    
    const auto& mapping = mappings[rowNumber];
    
    // Use white text with good contrast on aluminum background
    g.setColour(juce::Colours::white.withAlpha(0.85f));
    g.setFont (juce::Font (juce::FontOptions (12.0f)));
    
    juce::String text;
    
    switch (columnId)
    {
        case ComponentNameColumn:
            text = mapping.componentName;
            break;
        default:
            // Other columns use custom components
            return;
    }
    
    g.drawText(text, 4, 0, width - 8, height, juce::Justification::centredLeft);
}

juce::Component* MidiCCEditor::refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, juce::Component* existingComponentToUpdate)
{
    if (rowNumber >= mappings.size())
        return nullptr;
    
    auto& mapping = mappings.getReference(rowNumber);
    
    // Only handle the CC Number column - Component Name is just painted text
    if (columnId == CCNumberColumn)
    {
        auto* editor = dynamic_cast<CCNumberEditor*>(existingComponentToUpdate);
        if (editor == nullptr)
            editor = new CCNumberEditor();
        
        editor->setValue(mapping.ccNumber >= 0 ? mapping.ccNumber : 0);
        editor->onValueChanged = [this, rowNumber](int value) { setCCMapping(rowNumber, value); };
        return editor;
    }
    
    return nullptr;
}

void MidiCCEditor::setCCMapping(int row, int ccNumber)
{
    if (row >= 0 && row < mappings.size())
    {
        mappings.getReference(row).ccNumber = ccNumber;
        // TODO: Update the actual component's CC mapping
    }
}

void MidiCCEditor::setVisible(bool shouldBeVisible)
{
    Component::setVisible(shouldBeVisible);
    drawerOpen = shouldBeVisible;
}

void MidiCCEditor::toggleDrawer()
{
    setVisible(!drawerOpen);
}

//==============================================================================
// MidiCCDrawer Implementation
//==============================================================================

MidiCCDrawer::MidiCCDrawer(Controller& ctrl) : controller(ctrl), editor(ctrl)
{
    addAndMakeVisible(editor);
    editor.setVisible(false);
    setSize(400, 0); // Start with 0 height
}

MidiCCDrawer::~MidiCCDrawer()
{
    // No timer to stop anymore
}

void MidiCCDrawer::paint(juce::Graphics& g)
{
    if (currentDrawerHeight > 0)
    {
        auto bounds = getLocalBounds();
        
        // Match the main UI aluminum styling
        g.setColour(juce::Colour::fromRGB(45, 48, 52));
        g.fillRect(bounds);

        // Subtle gradient for depth
        juce::ColourGradient grad(
            juce::Colour::fromRGB(50, 53, 57), 0.0f, 0.0f,
            juce::Colour::fromRGB(40, 43, 47), 0.0f, (float)getHeight(),
            false);
        g.setGradientFill(grad);
        g.fillRect(bounds);
        
        // Draw a separator line at the top with aluminum styling
        g.setColour(juce::Colours::white.withAlpha(0.2f));
        g.drawHorizontalLine(0, 0, (float)getWidth());
        
        // Subtle shadow line
        g.setColour(juce::Colours::black.withAlpha(0.3f));
        g.drawHorizontalLine(1, 0, (float)getWidth());
    }
}

void MidiCCDrawer::resized()
{
    if (currentDrawerHeight > 0)
    {
        editor.setBounds(0, 0, getWidth(), currentDrawerHeight);
    }
}

void MidiCCDrawer::toggleDrawer()
{
    isDrawerOpen = !isDrawerOpen;
    
    // Set the height immediately without animation
    currentDrawerHeight = isDrawerOpen ? targetDrawerHeight : 0;
    
    if (isDrawerOpen)
    {
        editor.setVisible(true);
        editor.refreshMappings();
    }
    else
    {
        editor.setVisible(false);
    }
    
    setSize(getWidth(), currentDrawerHeight);
    resized();
    
    // Notify parent about size change
    if (onSizeChanged)
    {
        onSizeChanged(currentDrawerHeight);
    }
    
    if (auto* parent = getParentComponent())
        parent->resized();
}

}
