#pragma once
#include "Electrum/Audio/Generator/Oscillator.h"
#include "Electrum/Audio/Modulator/AHDSR.h"
#include "Electrum/Common.h"
#include "Electrum/Identifiers.h"
#include "Electrum/Shared/ElectrumState.h"
#include "Electrum/Shared/GraphingData.h"

//========================================================
#define QUICK_KILL_MS 4.0f
// forward declaration for the envelope
class ElectrumVoice;

class VoiceGateEnvelope {
private:
  ElectrumVoice* const parent;
  bool gate;
  bool forceKillQuick;
  size_t samplesSinceGateChange;
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
  bool isFinished() const { return !gate && lastOutput == 0.0f; }
  void killQuick() {
    forceKillQuick = true;
    gate = false;
  }

private:
  bool parentIsFinished();
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
  // oscillator stuff
  juce::OwnedArray<WavetableOscillator> oscs;
  osc_mod_t oscModState[NUM_OSCILLATORS];

  // envelope stuff
  juce::OwnedArray<AHDSREnvelope> envs;

public:
  const int voiceIndex;
  ElectrumVoice(ElectrumState* s, int idx);
  // sample rate update callback
  void sampleRateSet(double sr);
  bool gateIsOn() const { return gate; }
  bool isBusy();
  void startNote(int note, float velocity);
  void stealNote(int note, float velocity);
  void stopNote();
  int getCurrentNote() const { return currentNote; }
  void renderNextSample(float& left, float& right, bool updateDests);
  // callback for gripping graph data
  void updateGraphData(GraphingData* gd);

private:
  // this gets called on the state pointer's ModMap for every
  // sample that we want to update the modulated parameters
  void _updateModDests(ModMap* map);
  void _applyNormalizedMod(float mod, int destID);
  float _normalizedModulationForDest(ModMap* map, int destID);
  float _currentModSrcVal(int src);
  friend class VoiceGateEnvelope;
};
