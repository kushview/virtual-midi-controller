#include "lookandfeel.hpp"

namespace vmc
{

    void LookAndFeel::drawLinearSlider(juce::Graphics &g, int x, int y, int width, int height,
                                       float sliderPos, float minSliderPos, float maxSliderPos,
                                       const juce::Slider::SliderStyle style, juce::Slider &slider)
    {
        // Draw the track
        auto trackWidth = 6.0f;
        juce::Rectangle<float> track;
        if (style == juce::Slider::LinearVertical || style == juce::Slider::ThreeValueVertical)
            track = juce::Rectangle<float>(x + width * 0.5f - trackWidth * 0.5f, y, trackWidth, (float)height);
        else
            track = juce::Rectangle<float>(x, y + height * 0.5f - trackWidth * 0.5f, (float)width, trackWidth);

        // Track background (matching rotary inner color)
        g.setColour(juce::Colour::fromRGB(42, 42, 42));
        g.fillRoundedRectangle(track, 3.0f);

        // Draw the fader cap (handle)
        float faderWidth = (style == juce::Slider::LinearVertical) ? width * 0.8f : 18.0f;
        float faderHeight = (style == juce::Slider::LinearVertical) ? 24.0f : height * 0.8f;

        juce::Rectangle<float> fader;
        if (style == juce::Slider::LinearVertical || style == juce::Slider::ThreeValueVertical)
            fader = juce::Rectangle<float>(x + (width - faderWidth) * 0.5f, sliderPos - faderHeight * 0.5f, faderWidth, faderHeight);
        else
            fader = juce::Rectangle<float>(sliderPos - faderWidth * 0.5f, y + (height - faderHeight) * 0.5f, faderWidth, faderHeight);

        // Fader body (darker outer ring like rotary)
        g.setColour(juce::Colour::fromRGB(28, 28, 28));
        g.fillRoundedRectangle(fader, 4.0f);

        // Inner fader area (matching rotary inner color)
        auto innerFader = fader.reduced(2.0f);
        g.setColour(juce::Colour::fromRGB(42, 42, 42));
        g.fillRoundedRectangle(innerFader, 3.0f);

        // Subtle shadow at top
        g.setColour(juce::Colours::black.withAlpha(0.2f));
        g.drawRoundedRectangle(fader, 4.0f, 1.0f);

        // Fader highlight (subtle like rotary)
        g.setColour(juce::Colours::white.withAlpha(0.12f));
        g.drawLine(fader.getX() + 2, fader.getY() + 3, fader.getRight() - 2, fader.getY() + 3, 1.5f);

        // Fader indicator line (matching rotary blue color)
        g.setColour(juce::Colour::fromRGB(64, 160, 255));
        float cy = fader.getCentreY();
        g.drawLine(fader.getX() + 4, cy, fader.getRight() - 4, cy, 2.5f);
    }

    // --- Rotary slider implementation ---

    void LookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height,
                                       float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle,
                                       juce::Slider &slider)
    {
        auto bounds = juce::Rectangle<float>((float)x, (float)y, (float)width, (float)height).reduced(2.0f);
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
        auto centre = bounds.getCentre();
        
        // Outer ring
        g.setColour(juce::Colour::fromRGB(28, 28, 28));
        g.fillEllipse(bounds);
        
        // Inner circle (slightly smaller)
        auto innerBounds = bounds.reduced(3.0f);
        g.setColour(juce::Colour::fromRGB(42, 42, 42));
        g.fillEllipse(innerBounds);
        
        // Subtle radial gradient for depth
        juce::ColourGradient grad(
            juce::Colours::black.withAlpha(0.15f), centre.x, centre.y,
            juce::Colours::transparentBlack, centre.x + radius, centre.y + radius,
            true);  // true = radial gradient
        g.setGradientFill(grad);
        g.fillEllipse(innerBounds);

        // Calculate indicator position
        float angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
        float dotOffset = radius * 0.75f;
        juce::Point<float> dotCenter = centre.getPointOnCircumference(dotOffset, angle);
        
        // Draw indicator dot
        float dotSize = 8.0f;  // Increased from 6.0f
        g.setColour(juce::Colour::fromRGB(64, 160, 255));
        g.fillEllipse(dotCenter.x - dotSize * 0.5f, 
                      dotCenter.y - dotSize * 0.5f,
                      dotSize, dotSize);
        
        // Small highlight on dot
        float highlightSize = dotSize * 0.4f;  // Will automatically scale with new dotSize
        g.setColour(juce::Colours::white.withAlpha(0.4f));
        g.fillEllipse(dotCenter.x - highlightSize * 0.5f,
                      dotCenter.y - highlightSize * 0.5f,
                      highlightSize, highlightSize);
    }
}

