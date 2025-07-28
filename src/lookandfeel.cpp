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

        g.setColour(juce::Colours::darkgrey.darker(0.2f));
        g.fillRoundedRectangle(track, 3.0f);

        // Draw the fader cap (handle)
        float faderWidth = (style == juce::Slider::LinearVertical) ? width * 0.8f : 18.0f;
        float faderHeight = (style == juce::Slider::LinearVertical) ? 24.0f : height * 0.8f;

        juce::Rectangle<float> fader;
        if (style == juce::Slider::LinearVertical || style == juce::Slider::ThreeValueVertical)
            fader = juce::Rectangle<float>(x + (width - faderWidth) * 0.5f, sliderPos - faderHeight * 0.5f, faderWidth, faderHeight);
        else
            fader = juce::Rectangle<float>(sliderPos - faderWidth * 0.5f, y + (height - faderHeight) * 0.5f, faderWidth, faderHeight);

        // Fader body
        g.setColour(juce::Colours::lightgrey.withAlpha(0.95f));
        g.fillRoundedRectangle(fader, 4.0f);

        // Fader outline
        g.setColour(juce::Colours::black.withAlpha(0.7f));
        g.drawRoundedRectangle(fader, 4.0f, 1.5f);

        // Fader highlight
        g.setColour(juce::Colours::white.withAlpha(0.18f));
        g.drawLine(fader.getX() + 2, fader.getY() + 3, fader.getRight() - 2, fader.getY() + 3, 2.0f);

        // Fader indicator line
        g.setColour(juce::Colours::darkred.withAlpha(0.85f));
        float cy = fader.getCentreY();
        g.drawLine(fader.getX() + 4, cy, fader.getRight() - 4, cy, 2.0f);
    }

    // --- Rotary slider implementation ---

    void LookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height,
                                       float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle,
                                       juce::Slider &slider)
    {
        auto bounds = juce::Rectangle<float>((float)x, (float)y, (float)width, (float)height).reduced(4.0f);
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
        auto centre = bounds.getCentre();

        // Base gradient (glossy effect)
        juce::ColourGradient baseGrad(
            juce::Colour::fromRGB(40, 42, 45), bounds.getTopLeft(),
            juce::Colour::fromRGB(25, 27, 30), bounds.getBottomRight(), true);
        g.setGradientFill(baseGrad);
        g.fillEllipse(bounds);

        // Gloss overlay
        juce::ColourGradient gloss(
            juce::Colours::white.withAlpha(0.3f), bounds.getX(), bounds.getY(),
            juce::Colours::transparentWhite, bounds.getX(), bounds.getCentreY(), false);
        g.setGradientFill(gloss);
        g.fillEllipse(bounds);

        // Modern tick marks
        g.setColour(juce::Colours::white.withAlpha(0.15f));
        const int numTicks = 9;
        for (int i = 0; i < numTicks; ++i)
        {
            float angle = rotaryStartAngle + (rotaryEndAngle - rotaryStartAngle) * (float)i / (numTicks - 1);
            float tickLen = radius * 0.1f;
            juce::Point<float> p1 = centre.getPointOnCircumference(radius - 2.0f, angle);
            juce::Point<float> p2 = centre.getPointOnCircumference(radius - tickLen - 2.0f, angle);
            g.drawLine(p1.x, p1.y, p2.x, p2.y, 1.0f);
        }

        // Draw pointer
        float angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
        float pointerLen = radius * 0.65f;
        juce::Point<float> pointerEnd = centre.getPointOnCircumference(pointerLen, angle);

        // Pointer glow effect
        g.setColour(juce::Colours::cyan.withAlpha(0.15f));
        g.drawLine(centre.x, centre.y, pointerEnd.x, pointerEnd.y, 4.5f);

        // Main pointer
        g.setColour(juce::Colour::fromRGB(0, 255, 255).withAlpha(0.85f));
        g.drawLine(centre.x, centre.y, pointerEnd.x, pointerEnd.y, 2.0f);

        // Center dot with gloss
        g.setColour(juce::Colours::black.withAlpha(0.8f));
        g.fillEllipse(centre.x - 3.5f, centre.y - 3.5f, 7.0f, 7.0f);
        g.setColour(juce::Colours::white.withAlpha(0.4f));
        g.fillEllipse(centre.x - 3.5f, centre.y - 3.5f, 4.0f, 2.5f);
    }

}
