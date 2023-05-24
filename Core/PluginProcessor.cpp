/*
  ==============================================================================
    This file contains the basic framework code for a JUCE plugin processor.
  ==============================================================================
*/

#include "PluginProcessor.h"

#include "PluginEditor.h"


//==============================================================================
ElectrumAudioProcessor::ElectrumAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(
          BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
      ),
#endif
state (*this, nullptr, IDs::ELECTRUM_STATE),
engine (&state)
{
#if PERFETTO
  MelatoninPerfetto::get().beginSession();
#endif
}

ElectrumAudioProcessor::~ElectrumAudioProcessor() 
{
#if PERFETTO
    MelatoninPerfetto::get().endSession();
#endif
}

//==============================================================================
const juce::String ElectrumAudioProcessor::getName() const 
{
  return JucePlugin_Name;
}

bool ElectrumAudioProcessor::acceptsMidi() const 
{
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool ElectrumAudioProcessor::producesMidi() const 
{
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool ElectrumAudioProcessor::isMidiEffect() const 
{
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double ElectrumAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int ElectrumAudioProcessor::getNumPrograms() 
{
  return 1;  // NB: some hosts don't cope very well if you tell them there are 0
             // programs, so this should be at least 1, even if you're not
             // really implementing programs.
}

int ElectrumAudioProcessor::getCurrentProgram() { return 0; }

void ElectrumAudioProcessor::setCurrentProgram(int index) {}

const juce::String ElectrumAudioProcessor::getProgramName(int index) 
{
  return {};
}

void ElectrumAudioProcessor::changeProgramName(int index, const juce::String &newName) {}

//==============================================================================
void ElectrumAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) 
{
  AudioSystem::setSampleRate(sampleRate);
  AudioSystem::setBlockSize(samplesPerBlock);
  DLog::log("Prepared to play with sample rate: " + String(sampleRate) + " and block size: " + String(samplesPerBlock));
}

void ElectrumAudioProcessor::releaseResources() 
{
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ElectrumAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const 
    {
#if JucePlugin_IsMidiEffect
  juce::ignoreUnused(layouts);
  return true;
#else
  // This is the place where you check if the layout is supported.
  // In this template code we only support mono or stereo.
  if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
      layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    return false;

    // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    return false;
#endif

  return true;
#endif
}
#endif

void ElectrumAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages) 
{
  juce::ScopedNoDenormals noDenormals;
  engine.processBlock(buffer, midiMessages);
}

//==============================================================================
bool ElectrumAudioProcessor::hasEditor() const 
{
  return true;  // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *ElectrumAudioProcessor::createEditor() 
{
  return new ElectrumAudioProcessorEditor(*this);
}

//==============================================================================
void ElectrumAudioProcessor::getStateInformation(juce::MemoryBlock &destData) 
{
  // You should use this method to store your parameters in the memory block.
  // You could do that either as raw data, or use the XML or ValueTree classes
  // as intermediaries to make it easy to save and load complex data.

}

void ElectrumAudioProcessor::setStateInformation(const void *data, int sizeInBytes) 
{
  // You should use this method to restore your parameters from this memory
  // block, whose contents will have been created by the getStateInformation()
  // call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor *JUCE_CALLTYPE createPluginFilter() 
{
  return new ElectrumAudioProcessor();
}