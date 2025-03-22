#pragma once
#include "Electrum/Shared/ElectrumState.h"
#include "juce_audio_basics/juce_audio_basics.h"
#include "Voice.h"
#include "juce_core/system/juce_PlatformDefs.h"

struct timed_midi_msg {
  int timestamp;
  juce::MidiMessage message;
};

#define DEST_UPDATE_INTERVAL 35
#define NUM_VOICES 24

class SynthEngine {
public:
  ElectrumState* const state;
  AudioSourceState audioSrc;

private:
  std::queue<timed_midi_msg> midiQueue;
  std::queue<ElectrumVoice*> sustainedVoices;
  void killSustainedVoices();
  // state
  juce::OwnedArray<ElectrumVoice> voices;
  uint32_t destUpdateIdx = 0;
  // functions
  void noteOn(int note, float velocity);
  void noteOff(int note);
  void renderNextSample(float& left, float& right, bool updateDests);

  ElectrumVoice* getFreeVoice();
  ElectrumVoice* getVoicePlayingNote(int note);

  int numBusyVoices();
  void updateParamsForBlock();
  // helpers for processBlock
  void loadMidiEvents(juce::MidiBuffer& midi);
  void handleMidiMessage(juce::MidiMessage& message);

public:
  SynthEngine(ElectrumState* s);
  void processBlock(juce::AudioBuffer<float>& audioBuf,
                    juce::MidiBuffer& midiBuf);
  void prepareToPlay(double sampleRate, size_t blockSize);
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthEngine)
};
