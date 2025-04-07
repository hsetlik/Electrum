#include "Electrum/PluginProcessor.h"

#include "Electrum/Audio/AudioUtil.h"
#include "Electrum/Common.h"
#include "Electrum/Identifiers.h"
#include "Electrum/PluginEditor.h"
namespace audio_plugin {
ElectrumAudioProcessor::ElectrumAudioProcessor()
    : AudioProcessor(
          BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
              ),
      tree(*this, nullptr),
      engine(&tree) {
}

ElectrumAudioProcessor::~ElectrumAudioProcessor() {}

const juce::String ElectrumAudioProcessor::getName() const {
  return JucePlugin_Name;
}

bool ElectrumAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool ElectrumAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool ElectrumAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;

#endif
}

double ElectrumAudioProcessor::getTailLengthSeconds() const {
  return 0.0;
}

int ElectrumAudioProcessor::getNumPrograms() {
  return 1;  // NB: some hosts don't cope very well if you tell them there are 0
  // programs, so this should be at least 1, even if you're not
  // really implementing programs.
}

int ElectrumAudioProcessor::getCurrentProgram() {
  return 0;
}

void ElectrumAudioProcessor::setCurrentProgram(int index) {
  juce::ignoreUnused(index);
}

const juce::String ElectrumAudioProcessor::getProgramName(int index) {
  juce::ignoreUnused(index);

  return {};
}

void ElectrumAudioProcessor::changeProgramName(int index,
                                               const juce::String& newName) {
  juce::ignoreUnused(index, newName);
}

void ElectrumAudioProcessor::prepareToPlay(double sampleRate,
                                           int samplesPerBlock) {
  // Use this method as the place to do any pre-playback
  // initialisation that you need..
  SampleRate::set(sampleRate);
  AudioUtil::updateTuningTables(sampleRate);
  engine.prepareToPlay(sampleRate, samplesPerBlock);
}

void ElectrumAudioProcessor::releaseResources() {
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
}

bool ElectrumAudioProcessor::isBusesLayoutSupported(
    const BusesLayout& layouts) const {
#if JucePlugin_IsMidiEffect
  juce::ignoreUnused(layouts);
  return true;
#else
  // This is the place where you check if the layout is supported.
  // In this template code we only support mono or stereo.
  // Some plugin hosts, such as certain GarageBand versions, will only
  // load plugins that support stereo bus layouts.
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

void ElectrumAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                          juce::MidiBuffer& midiMessages) {
  juce::ScopedNoDenormals noDenormals;
  // This is the place where you'd normally do the guts of your plugin's
  // audio processing...
  // Make sure to reset the state if your inner loop is processing
  // Alternatively, you can process the samples with the channels
  // interleaved by keeping the same state.
  tree.modulations.updateMap(tree.getModulationTree());
  engine.processBlock(buffer, midiMessages);
}

bool ElectrumAudioProcessor::hasEditor() const {
  return true;  // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ElectrumAudioProcessor::createEditor() {
  return new ElectrumProcessorEditor(*this);
}

void ElectrumAudioProcessor::getStateInformation(

    juce::MemoryBlock& destData) {
  auto stateCpy = tree.copyState();
  String stateXml = stateCpy.toXmlString();
  destData.replaceAll(stateXml.toRawUTF8(), stateXml.getNumBytesAsUTF8());
}

void ElectrumAudioProcessor::setStateInformation(const void* data,
                                                 int sizeInBytes) {
  // You should use this method to restore your parameters from this memory
  // block, whose contents will have been created by the getStateInformation()
  // call.
  String xmlStr((const char*)data, (size_t)sizeInBytes);
  auto vt = ValueTree::fromXml(xmlStr);
  jassert(vt.isValid());
  tree.replaceState(vt);
}

}  // namespace audio_plugin
// This creates new instances of the plugin.
// This function definition must be in the global namespace.
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
  return new audio_plugin::ElectrumAudioProcessor();
}
