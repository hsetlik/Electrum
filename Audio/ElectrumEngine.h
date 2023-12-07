#pragma once
#include "../Parameters/ElectrumValueTree.h"
#include "ElectrumVoice.h"
#include "Modulators/Oscillator.h"
#include <stack>
#define NUM_VOICES 24
#define DEST_UPDATE_INTERVAL 50

class ElectrumEngine
{
private:
  struct TimestampedMidiMessage {
    int timestamp;
    MidiMessage message;
  };
  std::queue<TimestampedMidiMessage> midiQueue;
  ModDestMap currentModulation;
  // state
  OwnedArray<ElectrumVoice> voices;
  uint32 destUpdateIdx;
  // functions
  void noteOn(int note, float velocity);
  void noteOff(int note);
  void renderNextSample(float &left, float &right, bool updateDests);

  ElectrumVoice *getFreeVoice();
  ElectrumVoice *getVoicePlayingNote(int note);
  ElectrumVoice *getVoiceForNote(int note);

  int numBusyVoices();
  void updateParamsForBlock();
  // helpers for processBlock
  void loadMidiEvents(MidiBuffer &midi);
  void handleMidiMessage(MidiMessage &message);

public:
  EVT *const state;
  ElectrumEngine(EVT *tree);
  // main callback
  void processBlock(AudioBuffer<float> &buffer, MidiBuffer &midi);
  // setup for audio stuff
  void prepareToPlay(double sampleRate, size_t blockSize)
  {
    for (auto v : voices)
      v->prepareToPlay(sampleRate, blockSize);
  }
};
