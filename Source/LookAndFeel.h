/*
  ==============================================================================

    LookAndFeel.h
    Created: 4 Mar 2024 9:43:24am
    Author:  Gyozo Kisteleki

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "config.h"


class KnobLookAndFeel_1 : public juce::LookAndFeel_V4
{
public:

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
        const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider&) override
    {
        auto radius = (float)juce::jmin(width / 2, height / 2) - 4.0f;
        auto centreX = (float)x + (float)width * 0.5f;
        auto centreY = (float)y + (float)height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        // fill
        //g.setColour(juce::Colour(0xffC6C7C2));
        g.setColour(KNOB_1_COLOR);
        g.fillEllipse(rx, ry, rw, rw);

        juce::Path p;
        auto pointerLength = radius * 0.33f;
        auto pointerThickness = 2.0f;
        p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));

        // pointer
        g.setColour(juce::Colours::white);
        g.fillPath(p);
    }
};


class KnobLookAndFeel_2 : public juce::LookAndFeel_V4
{
public:
    KnobLookAndFeel_2()
    {
        //setColour(juce::Slider::thumbColourId, juce::Colours::red);
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
        const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider&) override
    {
        auto radius = (float)juce::jmin(width / 2, height / 2) - 4.0f;
        auto centreX = (float)x + (float)width * 0.5f;
        auto centreY = (float)y + (float)height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        // fill
        g.setColour(juce::Colour(KNOB_2_COLOR));
        g.fillEllipse(rx, ry, rw, rw);

        juce::Path p;
        auto pointerLength = radius * 0.33f;
        auto pointerThickness = 2.0f;
        p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));

        // pointer
        g.setColour(juce::Colours::white);
        g.fillPath(p);
    }
};


class ButtonLookAndFeel : public juce::LookAndFeel_V4
{
    //juce::Colour button_color = juce::Colours::black;

public:
    void drawToggleButton(juce::Graphics& g,
        juce::ToggleButton& toggleButton,
        bool /*shouldDrawButtonAsHighlighted*/,
        bool /*shouldDrawButtonAsDown*/) override
    {
        auto bounds = toggleButton.getLocalBounds().reduced(2);
        auto buttonIsOn = toggleButton.getToggleState();

        g.setColour(juce::Colours::black);
        toggleButton.setAlpha(buttonIsOn ? 0.5f : 0.01f);
        g.fillRoundedRectangle(bounds.toFloat(), 1);

    }
};


class SliderLookAndFeel : public juce::LookAndFeel_V4
{

public:
    SliderLookAndFeel()
    {
        setColour(juce::Slider::trackColourId, juce::Colours::black.withAlpha(0.1f));
        setColour(juce::Slider::backgroundColourId, juce::Colours::black.withAlpha(0.1f));
        setColour(juce::Slider::thumbColourId, juce::Colour(0xffC6C7C2));
    }

};