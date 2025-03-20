#pragma once
#include "../Wavetable.h"
#include "Electrum/Audio/Generator/Oscillator.h"
#include "Electrum/Common.h"
#include "Electrum/Identifiers.h"
#include "Electrum/Shared/ElectrumState.h"

// have the engine class own one of these and each
// voice gets a pointer to it
struct AudioSourceState {
  Wavetable wOsc[NUM_OSCILLATORS];
};

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
    float posMod = 0.0f;
    float levelMod = 0.0f;
    float panMod = 0.0f;
    float coarseMod = 0.0f;
    float fineMod = 0.0f;
  };
  //---------------------------------------------
  ElectrumState* const state;
  AudioSourceState* const audioState;
  bool gate = false;
  // killQuick stuff
  bool inQuickKill = false;
  int queuedNote = 69;
  float queuedVelocity = 0.0f;
  // note state stuff
  int currentNote = 69;
  float currentNoteVelocity = 0.0f;
  VoiceGateEnvelope vge;
  // oscillator stuff
  juce::OwnedArray<WavetableOscillator> wOscs;
  osc_mod_t oscModState[NUM_OSCILLATORS];

public:
  const int voiceIndex;
  ElectrumVoice(ElectrumState* s, AudioSourceState* a, int idx);
  bool gateIsOn() const { return gate; }
  bool isBusy();
  void startNote(int note, float velocity);
  void stealNote(int note, float velocity);
  void stopNote();
  int getCurrentNote() const { return currentNote; }
  void renderNextSample(float& left, float& right, bool updateDests);
};
