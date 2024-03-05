/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class Hyper76AudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    Hyper76AudioProcessor();
    ~Hyper76AudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    // Encapsulate this better in production code!
    juce::AudioProcessorValueTreeState state;

private:
    //==============================================================================
    int delayBufferPos = 0;
    juce::AudioBuffer<float> delayBuffer;
    
    juce::dsp::StateVariableTPTFilter<float> bassFilter;
    juce::dsp::Chorus<float> myChorus;
    juce::dsp::Reverb myReverb;
    juce::dsp::Reverb::Parameters reverbParams;
    
    juce::dsp::ProcessSpec mySpec;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Hyper76AudioProcessor)
};
