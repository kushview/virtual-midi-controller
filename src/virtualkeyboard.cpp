/*
    This file is part of Virtual MIDI Controller
    Copyright (c) 2019  Kushview, LLC.  All rights reserved.
*/

#include "virtualkeyboard.hpp"

namespace vmc {

StyledMidiKeyboardComponent::StyledMidiKeyboardComponent(juce::MidiKeyboardState& state, Orientation orientation)
    : juce::MidiKeyboardComponent(state, orientation)
{
}

void StyledMidiKeyboardComponent::drawWhiteNote(int midiNoteNumber, juce::Graphics& g, juce::Rectangle<float> area,
                                               bool isDown, bool isOver, juce::Colour lineColour, juce::Colour textColour)
{
    // White key background
    juce::Colour keyColour = juce::Colours::white;
    
    if (isDown)
    {
        // Pressed state - use the blue highlight color
        keyColour = juce::Colour::fromRGB(64, 160, 255);
    }
    else if (isOver)
    {
        // Hover state - subtle blue tint
        keyColour = juce::Colours::white.overlaidWith(juce::Colour::fromRGB(64, 160, 255).withAlpha(0.1f));
    }
    
    g.setColour(keyColour);
    g.fillRect(area);
    
    // Key outline - match the aluminum theme's dark borders
    g.setColour(juce::Colour::fromRGB(28, 28, 28));
    g.drawRect(area, 1.0f);
    
    // Subtle inner shadow for depth
    if (!isDown)
    {
        g.setColour(juce::Colours::black.withAlpha(0.1f));
        g.drawLine(area.getX(), area.getY(), area.getRight(), area.getY(), 1.0f);
        g.drawLine(area.getX(), area.getY(), area.getX(), area.getBottom(), 1.0f);
    }
}

void StyledMidiKeyboardComponent::drawBlackNote(int midiNoteNumber, juce::Graphics& g, juce::Rectangle<float> area,
                                               bool isDown, bool isOver, juce::Colour noteFillColour)
{
    // Black key background - match the aluminum theme's dark colors
    juce::Colour keyColour = juce::Colour::fromRGB(28, 28, 28);
    
    if (isDown)
    {
        // Pressed state - use the blue highlight color
        keyColour = juce::Colour::fromRGB(64, 160, 255);
    }
    else if (isOver)
    {
        // Hover state - lighter dark gray with blue tint
        keyColour = juce::Colour::fromRGB(35, 35, 35).overlaidWith(juce::Colour::fromRGB(64, 160, 255).withAlpha(0.2f));
    }
    
    g.setColour(keyColour);
    g.fillRoundedRectangle(area, 2.0f);
    
    // Black key highlight - subtle top edge like other controls
    if (!isDown)
    {
        g.setColour(juce::Colours::white.withAlpha(0.08f));
        g.drawLine(area.getX() + 1, area.getY() + 1, area.getRight() - 1, area.getY() + 1, 1.0f);
    }
    
    // Outer border
    g.setColour(juce::Colours::black.withAlpha(0.3f));
    g.drawRoundedRectangle(area, 2.0f, 1.0f);
}

}
