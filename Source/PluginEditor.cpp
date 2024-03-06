/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Hyper76AudioProcessorEditor::Hyper76AudioProcessorEditor(Hyper76AudioProcessor& p)
    : juce::AudioProcessorEditor(&p), audioProcessor(p),
    knob1Attachment(p.state, "gain",   knob1),
    knob2Attachment(p.state, "bass",   knob2),
    knob3Attachment(p.state, "depth",  knob3),
    knob4Attachment(p.state, "rate",   knob4),
    knob5Attachment(p.state, "revdly", knob5),
    knob6Attachment(p.state, "damp",   knob6),
    knob7Attachment(p.state, "size",   knob7),
    knob8Attachment(p.state, "width",  knob8),

    sliderAttachment(p.state, "feedback", slider),

    button1Attachment(p.state, "chorus", button1),
    button2Attachment(p.state, "filter", button2),
    button3Attachment(p.state, "double", button3),
    button4Attachment(p.state, "bypass", button4)
{

    slider.setSliderStyle (juce::Slider::SliderStyle::LinearHorizontal);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    slider.setLookAndFeel(&SliderLaf);
    addAndMakeVisible(slider);

    for (auto* knob : {
        &knob1, 
        &knob2, 
        &knob5, 
        &knob6, 
    })
    {
        knob->setSliderStyle(juce::Slider::SliderStyle::Rotary);
        knob->setTextBoxStyle (juce::Slider::NoTextBox, true, 0, 0);
        knob->setLookAndFeel(&KnobLaf_1);
        addAndMakeVisible (knob);
    }

    for (auto* knob : {
        &knob3, 
        &knob4, 
        &knob7, 
        &knob8, 
    })
    {
        knob->setSliderStyle(juce::Slider::SliderStyle::Rotary);
        knob->setTextBoxStyle (juce::Slider::NoTextBox, true, 0, 0);
        knob->setLookAndFeel(&KnobLaf_2);
        addAndMakeVisible (knob);
    }

    for (auto* button : { &button1, &button2, /*&button3, */&button4})
    {
        button->setLookAndFeel(&ButtonLaf_1);
        addAndMakeVisible (button);
    }

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (WINDOW_WIDTH, WINDOW_HEIGHT);
}

Hyper76AudioProcessorEditor::~Hyper76AudioProcessorEditor()
{
}

//==============================================================================
void Hyper76AudioProcessorEditor::paint (juce::Graphics& g)
{
    //g.fillAll (juce::Colour (0xff121212));
    background = juce::ImageCache::getFromMemory(BinaryData::background_jpg, BinaryData::background_jpgSize);
    g.drawImageWithin(background, 0, 0, getWidth(), getHeight(), juce::RectanglePlacement::stretchToFit);

}

void Hyper76AudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    knob1   .setBounds(COLUMN_1, UPPER_ROW_Y,    KNOB_DIAMETER, KNOB_DIAMETER);
    knob2   .setBounds(COLUMN_2, UPPER_ROW_Y,    KNOB_DIAMETER, KNOB_DIAMETER);
    knob3   .setBounds(COLUMN_1, LOWER_ROW_Y,    KNOB_DIAMETER, KNOB_DIAMETER);
    knob4   .setBounds(COLUMN_2, LOWER_ROW_Y,    KNOB_DIAMETER, KNOB_DIAMETER);
    knob5   .setBounds(COLUMN_3, LOLOWER_ROW_Y,  KNOB_DIAMETER, KNOB_DIAMETER);
    knob6   .setBounds(COLUMN_4-5, LOLOWER_ROW_Y+5,  KNOB_DIAMETER-10, KNOB_DIAMETER-10);
    knob7   .setBounds(COLUMN_5, LOWER_ROW_Y,    KNOB_DIAMETER, KNOB_DIAMETER);
    knob8   .setBounds(COLUMN_6, LOWER_ROW_Y,    KNOB_DIAMETER, KNOB_DIAMETER);

    slider  .setBounds(SLIDER_X, SLIDER_Y, SLIDER_W, SLIDER_H);
    
    button1 .setBounds(BUTTON_1X,  BUTTON_1Y,BUTTON_W,  BUTTON_H);
    button2 .setBounds(BUTTON_1X+1*BUTTON_D, BUTTON_1Y, BUTTON_W, BUTTON_H);
    //button3 .setBounds(BUTTON_1X+2*BUTTON_D, BUTTON_1Y, BUTTON_W, BUTTON_H);
    button4 .setBounds(BUTTON_1X+3*BUTTON_D, BUTTON_1Y, BUTTON_W, BUTTON_H);

}
