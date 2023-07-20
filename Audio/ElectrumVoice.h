#pragma once
#include "Modulators/Envelope.h"
#include "Modulators/Oscillator.h"
#include "../Parameters/ElectrumValueTree.h"
#include "../Parameters/MathUtil.h"
#include "Generators/WavetableOscillator.h"
#include "Processors/FilterProcessor.h"

// forward declaration, implementation at the bottom
class VoiceGateEnvelope;

class ElectrumVoice
{
private:

    EVT* const state;
    ModDestMap* const modMap;
    const int index;
    int currentNote;
    float currentNoteVelocity;
    bool gate;
    PlaceholderEnvelope env;
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
    size_t quickKillSamples;
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
    // this controls the default organ envelope that just makes stuff work
class VoiceGateEnvelope
{
private:
  ElectrumVoice* const parent;
  bool gate;
  size_t samplesSinceGateChange;
  float lastOutput;
public:
  VoiceGateEnvelope(ElectrumVoice* parent);

  void tick();
  float getCurrentSample() { return lastOutput; }

};
