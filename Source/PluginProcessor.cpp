/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Hyper76AudioProcessor::Hyper76AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
    , state (*this, nullptr, "STATE", {
        std::make_unique<juce::AudioParameterFloat> ("gain",     "Gain",           0.0f, 1.0f, 1.0f),
        std::make_unique<juce::AudioParameterInt>   ("bass",     "Bass",           50,   1000, 500),
        std::make_unique<juce::AudioParameterFloat> ("depth",    "Depth",          0.0f, 1.0f, 0.5f),
        std::make_unique<juce::AudioParameterFloat> ("rate",     "Rate",           0.0f, 2.0f, 0.5f),
        std::make_unique<juce::AudioParameterFloat> ("revdly",   "Rev/Dly",        0.0f, 1.0f, 0.5f),
        std::make_unique<juce::AudioParameterFloat> ("damp",     "Damp",           0.0f, 1.0f, 0.5f),
        std::make_unique<juce::AudioParameterFloat> ("size",     "Size",           0.0f, 1.0f, 0.5f),
        std::make_unique<juce::AudioParameterFloat> ("width",    "Width",          0.0f, 1.0f, 0.5f),


        std::make_unique<juce::AudioParameterFloat> ("feedback", "Delay Feedback",   0.0f, 1.0f, 0.35f),

        std::make_unique<juce::AudioParameterBool>  ("chorus",   "Chorus",  false),
        std::make_unique<juce::AudioParameterBool>  ("filter",   "Filter",  false),
        std::make_unique<juce::AudioParameterBool>  ("double",   "Double",  true),
        std::make_unique<juce::AudioParameterBool>  ("bypass",   "Bypass",  true)
    })
{
    myBassptr = dynamic_cast<juce::AudioParameterInt*>  (state.getParameter("bass"));
    myDampptr = dynamic_cast<juce::AudioParameterFloat*>(state.getParameter("damp"));
}

Hyper76AudioProcessor::~Hyper76AudioProcessor()
{
}

//==============================================================================
const juce::String Hyper76AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Hyper76AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Hyper76AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Hyper76AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Hyper76AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Hyper76AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Hyper76AudioProcessor::getCurrentProgram()
{
    return 0;
}

void Hyper76AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Hyper76AudioProcessor::getProgramName (int index)
{
    return {};
}

void Hyper76AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void Hyper76AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    int delayMilliseconds = 200;
    auto delaySamples = (int) std::round (sampleRate * delayMilliseconds / 1000.0);
    delayBuffer.setSize (2, delaySamples);
    delayBuffer.clear();
    delayBufferPos = 0;
    
    mySpec.sampleRate = sampleRate;
    mySpec.maximumBlockSize = samplesPerBlock;
    mySpec.numChannels = getTotalNumOutputChannels();

    myChorus.prepare(mySpec);
    myChorus.reset();

    reverbParams.dryLevel = 0.1;
    myReverb.setParameters(reverbParams);
    myReverb.prepare(mySpec);
    myReverb.reset();

    bassFilter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
    bassFilter.setResonance(0.707);
    bassFilter.prepare(mySpec);
    bassFilter.reset();
}

void Hyper76AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Hyper76AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif


void Hyper76AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.

    float gain      = state.getParameter ("gain")    ->getValue();
    float bass      = state.getParameter ("bass")    ->getValue();

    float depth     = state.getParameter ("depth")   ->getValue();
    float rate      = state.getParameter ("rate")    ->getValue();

    float revdly    = state.getParameter ("revdly")  ->getValue();
    
    reverbParams.damping = 1-(myDampptr->get());
    
    float feedback = state.getParameter ("feedback") ->getValue();
    
    bool  chorus   = state.getParameter ("chorus")   ->getValue();
    bool  filter   = state.getParameter ("filter")   ->getValue();
    bool  doubler  = state.getParameter ("double")   ->getValue();
    bool  bypass   = state.getParameter ("bypass")   ->getValue();

    float mix = revdly;
    reverbParams.wetLevel = 0.2 - 0.2 * revdly;

    //reverbParams.dryLevel = state.getParameter("config4")->getValue();
    reverbParams.roomSize = state.getParameter("size") ->getValue();
    reverbParams.width =    state.getParameter("width")->getValue();

    auto myBlock = juce::dsp::AudioBlock<float>(buffer);
    auto myContext = juce::dsp::ProcessContextReplacing<float>(myBlock);


    if (!bypass)
    {
        
        // CHORUS

        myChorus.setRate(rate);
        myChorus.setDepth(depth);

        if (chorus)
            myChorus.process(myContext);

        // DELAY

        int delayBufferSize = delayBuffer.getNumSamples();

        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            float* channelData = buffer.getWritePointer (channel);
            int delayPos = delayBufferPos;

            for (int i = 0; i < buffer.getNumSamples(); ++i)
            {
                float drySample = channelData[i];

                float delaySample = delayBuffer.getSample (channel, delayPos) * feedback;
                delayBuffer.setSample (channel, delayPos, drySample + delaySample);

                delayPos++;
                if (delayPos == delayBufferSize)
                    delayPos = 0;

                channelData[i] = (drySample * (1.0f - mix)) + (delaySample * mix);
                channelData[i] *= gain;
            }

        }

        delayBufferPos += buffer.getNumSamples();
        if (delayBufferPos >= delayBufferSize)
            delayBufferPos -= delayBufferSize;
        
        // REVERB
        myReverb.setParameters(reverbParams);
        myReverb.process(myContext);
    
        // FILTERS
        if (filter)
        {
            bassFilter.setCutoffFrequency(1050-(myBassptr->get()));
            bassFilter.process(myContext);
        }
    }

}

//==============================================================================
bool Hyper76AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Hyper76AudioProcessor::createEditor()
{
    return new Hyper76AudioProcessorEditor (*this);
    //return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void Hyper76AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    if (auto xmlState = state.copyState().createXml())
        copyXmlToBinary (*xmlState, destData);
}

void Hyper76AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    
    if (auto xmlState = getXmlFromBinary (data, sizeInBytes))
        state.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Hyper76AudioProcessor();
}
