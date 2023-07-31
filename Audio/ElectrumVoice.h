#pragma once
#include "Modulators/AHDSREnvelope.h"
#include "Modulators/Oscillator.h"
#include "../Parameters/ElectrumValueTree.h"
#include "../Parameters/MathUtil.h"
#include "Generators/WavetableOscillator.h"
#include "Processors/FilterProcessor.h"

// forward declaration for the envelope
class ElectrumVoice;

class VoiceGateEnvelope
{
private:
  ElectrumVoice* const parent;
  bool gate;
  bool forceKillQuick;
  size_t samplesSinceGateChange;
  float lastOutput;
  float levelDelta()
  {
      return (float)AudioSystem::getSampleRate() / (QUICK_KILL_MS / 1000.0f);
  }
public:
  VoiceGateEnvelope(ElectrumVoice* parent);
  void tick();
  float getCurrentSample() { return lastOutput; }
  void start();
  void end() { gate = false;}
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

    EVT* const state;
    ModDestMap* const modMap;
    const int index;
    int currentNote;
    float currentNoteVelocity;
    bool gate;
    VoiceGateEnvelope vge;
    PlaceholderOsc osc;
    OwnedArray<WavetableOscillator> oscs;
    OwnedArray<AHDSREnvelope> envs;
    SynthFilter filter;
    float baseFilterCutoff;
    float baseFilterRes;
    float baseFilterMix;
    float baseFilterTracking;
    String baseFilterType;
    // helper function for renderNextSample, deals with the filtering
    float filterSample(float input);
    // killQuick stuff
    bool inQuickKill;
    int queuedNote;
    float queuedVelocity;
public:
    ElectrumVoice(EVT* tree, ModDestMap* map, int idx);
    void prepareToPlay(double sampleRate, size_t blockSize)
    {
      filter.prepare(sampleRate, blockSize, 1);
    }
    // returns whether the voice can start a new note
    bool gateIsOn() { return gate; }
    bool isBusy();
    void startNote(int note, float velocity);
    void stealNote(int note, float velocity);
    void stopNote();
    void updateForBlock();

    int getCurrentNote() { return currentNote; }
    //called to get the current value of the given modulation source for this voice
    float getModValueForSample(const String& srcID);
    //
    float getCurrentModDestValue(const String& destID);

    //called for each sample on audio thread
    void renderNextSample(float& left, float& right);
    int getIndex() const { return index; }
    friend class VoiceGateEnvelope;
};
//=======================================================

