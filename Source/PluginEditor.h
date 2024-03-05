/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================

  knobs indexed from left-right, up-down

  1 GAIN
  2 bass
  3 chrDepth
  4 chrRate
  5 RvbDlyMix
  6 rvbDAMP
  7 rvbSize
  8 rvbWidth

  slider: dlyFeedback

  buttons from left-right

  1 filters off
  2 chorus off
  3 double feedback
  4 bypass

*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "config.h"
#include "LookAndFeel.h"

//==============================================================================
/**
*/
class Hyper76AudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    Hyper76AudioProcessorEditor (Hyper76AudioProcessor&);
    ~Hyper76AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Hyper76AudioProcessor& audioProcessor;
    
    juce::Image background;

    juce::Slider slider;
    juce::Slider knob1, knob2, knob3, knob4, knob5, knob6, knob7, knob8;
    juce::ToggleButton button1, button2, button3, button4;

    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    SliderAttachment sliderAttachment;
    SliderAttachment knob1Attachment,
                     knob2Attachment,
                     knob3Attachment,
                     knob4Attachment,
                     knob5Attachment,
                     knob6Attachment,
                     knob7Attachment,
                     knob8Attachment;

    KnobLookAndFeel_1 KnobLaf_1;
    KnobLookAndFeel_2 KnobLaf_2;

    ButtonLookAndFeel ButtonLaf_1;
    SliderLookAndFeel SliderLaf;


    juce::AudioProcessorValueTreeState::ButtonAttachment
        button1Attachment,
        button2Attachment,
        button3Attachment,
        button4Attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Hyper76AudioProcessorEditor)
};
