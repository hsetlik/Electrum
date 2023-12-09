#pragma once
#include "../Parameters/ElectrumValueTree.h"
#include "../Parameters/MathUtil.h"
#include "Generators/WavetableOscillator.h"
#include "Modulators/AHDSREnvelope.h"
#include "Modulators/Oscillator.h"
#include "Processors/FilterProcessor.h"

// forward declaration for the envelope
class ElectrumVoice;

class VoiceGateEnvelope
{
private:
  ElectrumVoice *const parent;
  bool gate;
  bool forceKillQuick;
  size_t samplesSinceGateChange;
  float lastOutput;
  float levelDelta()
  {
    return (float)AudioSystem::getSampleRate() / (QUICK_KILL_MS / 1000.0f);
  }

public:
  VoiceGateEnvelope(ElectrumVoice *parent);
  void tick();
  float getCurrentSample() { return lastOutput; }
  void start();
  void end() { gate = false; }
  bool isFinished() { return !gate && lastOutput == 0; }
  void killQuick()
  {
    forceKillQuick = true;
    gate = false;
  }

private:
  bool parentIsFinished();
};

class ElectrumVoice
{
private:
  // helps keep track of the oscillator mod values
  struct OscModValues {
    float posMod;
    float levelMod;
    float panMod;
    float coarseMod;
    float fineMod;
  };

  EVT *const state;
  ModDestMap *const modMap;
  const int index;
  int currentNote;
  float currentNoteVelocity;
  bool gate;
  VoiceGateEnvelope vge;
  PlaceholderOsc osc;
  OwnedArray<WavetableOscillator> oscs;
  OscModValues oscState[NUM_OSCILLATORS];
  OwnedArray<AHDSREnvelope> envs;
  SynthFilter filter;
  float baseFilterCutoff;
  float currentCutoff;
  float baseFilterRes;
  float currentRes;
  float baseFilterMix;
  float currentFilterMix;
  float baseFilterTracking;
  float currentFilterTracking;
  String baseFilterType;
  // helper function for renderNextSample, deals with the filtering
  float filterSample(float input, bool updateDests);
  void filterSampleStereo(float &left, float &right, bool updateDests);
  // killQuick stuff
  bool inQuickKill;
  int queuedNote;
  float queuedVelocity;

public:
  ElectrumVoice(EVT *tree, ModDestMap *map, int idx);
  void prepareToPlay(double sampleRate, size_t blockSize)
  {
    filter.prepare(sampleRate, blockSize, 2);
  }
  // returns whether the voice can start a new note
  bool gateIsOn() { return gate; }
  bool isBusy();
  void startNote(int note, float velocity);
  void stealNote(int note, float velocity);
  void stopNote();
  void updateForBlock();

  int getCurrentNote() { return currentNote; }
  // called to get the current value of the given modulation source for this
  // voice
  float getModValueForSample(const String &srcID);
  //
  float getCurrentModDestValue(const String &destID);

  // called for each sample on audio thread
  void renderNextSample(float &left, float &right, bool updateDests);
  int getIndex() const { return index; }
  friend class VoiceGateEnvelope;
};
//=======================================================
