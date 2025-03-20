#include "Electrum/Audio/Synth/Engine.h"
#include "juce_core/juce_core.h"

SynthEngine::SynthEngine(ElectrumState* s) : state(s) {}

void SynthEngine::processBlock(juce::AudioBuffer<float>& audioBuf,
                               juce::MidiBuffer& midiBuf) {
  juce::ignoreUnused(audioBuf, midiBuf);
}

void SynthEngine::prepareToPlay(double sampleRate, size_t blockSize) {
  juce::ignoreUnused(sampleRate, blockSize);
}
//===================================================
