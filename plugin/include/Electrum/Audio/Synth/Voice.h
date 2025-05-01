#pragma once
#include "Electrum/Audio/Filters/RollingRMS.h"
#include "Electrum/Audio/Filters/VoiceFilter.h"
#include "Electrum/Audio/Generator/Oscillator.h"
#include "Electrum/Audio/Modulator/AHDSR.h"
#include "Electrum/Audio/Modulator/LFO.h"
#include "Electrum/Common.h"
#include "Electrum/Identifiers.h"
#include "Electrum/Shared/ElectrumState.h"
#include "Electrum/Shared/GraphingData.h"
#include "juce_audio_basics/juce_audio_basics.h"

//========================================================
#define QUICK_KILL_MS 4.0f
// forward declaration for the envelope
class ElectrumVoice;

class VoiceGateEnvelope {
private:
  ElectrumVoice* const parent;
  bool gate;
  bool forceKillQuick;
  // size_t samplesSinceGateChange;
  float lastOutput;
  float levelDelta() const {
    return SampleRate::getf() / (QUICK_KILL_MS / 1000.0f);
  }

public:
  VoiceGateEnvelope(ElectrumVoice* parent);
  void tick();
  float getCurrentSample() const { return lastOutput; }
  void start();
  void end() { gate = false; }
  bool isFinished() const;
  void killQuick() {
    forceKillQuick = true;
    gate = false;
  }

private:
  bool parentIsFinished();
};

class FilterSumHandler {
private:
  std::array<float, 6> data = {};

public:
  FilterSumHandler() = default;
  // clear all the sums for the next sample
  void clear() { std::fill(data.begin(), data.end(), 0.0f); }
  void addToFilter1(float l, float r) {
    data[0] += l;
    data[1] += r;
  }
  void addToFilter2(float l, float r) {
    data[2] += l;
    data[3] += r;
  }
  void addToDry(float l, float r) {
    data[4] += l;
    data[5] += r;
  }
  float* filterLeft(int f);
  float* filterRight(int f);
  // call this with the references to the passed in samples
  void addCurrentSumTo(float& left, float& right) const;
  float getLeftSum() const { return data[0] + data[2] + data[4]; }
  float getRightSum() const { return data[1] + data[3] + data[5]; }
};

//========================================================
class ElectrumVoice {
private:
  struct osc_mod_t {
    float coarseMod = 0.0f;
    float fineMod = 0.0f;
    float posMod = 0.0f;
    float levelMod = 0.0f;
    float panMod = 0.0f;
  };
  //---------------------------------------------
  ElectrumState* const state;
  bool gate = false;
  // note state stuff
  int currentNote = 69;
  float currentNoteVelocity = 0.0f;
  // killQuick stuff
  bool inQuickKill = false;
  int queuedNote = 69;
  float queuedVelocity = 0.0f;
  VoiceGateEnvelope vge;
  bool wasBusy = false;
  // oscillators
  juce::OwnedArray<WavetableOscillator> oscs;
  osc_mod_t oscModState[NUM_OSCILLATORS];
  // envelopes
  juce::OwnedArray<AHDSREnvelope> envs;
  // LFOs
  juce::OwnedArray<VoiceLFO> lfos;
  // filters
  juce::OwnedArray<VoiceFilter> filters;
  FilterSumHandler filterSums;
  // RMS meter
  RollingRMS rms;

public:
  const int voiceIndex;
  ElectrumVoice(ElectrumState* s, int idx);
  // call this once a block on active voices
  // for the filters
  void updateForBlock();
  // sample rate update callback
  void sampleRateSet(double sr);
  bool gateIsOn() const { return gate; }
  bool isBusy() const;
  void startNote(int note, float velocity);
  void stealNote(int note, float velocity);

  void stopNote();
  int getCurrentNote() const { return currentNote; }
  void renderNextSample(float& left, float& right, bool updateDests);
  // callback for gripping graph data
  void updateGraphData(GraphingData* gd);

private:
  void addToFilterSums(float oscL, float oscR, int oscID);

  // this gets called on the state pointer's ModMap for every
  // sample that we want to update the modulated parameters
  void _updateModDests(ModMap* map);
  void _applyNormalizedMod(float mod, int destID);
  float _normalizedModulationForDest(ModMap* map, int destID);
  float _currentModSrcVal(int src);
  friend class VoiceGateEnvelope;
};
