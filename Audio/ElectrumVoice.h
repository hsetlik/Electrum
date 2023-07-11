#pragma once
#include "Modulators/Envelope.h"
#include "Modulators/AHDSREnvelope.h"
#include "Modulators/Oscillator.h"
#include "../Parameters/ElectrumValueTree.h"
#include "../Parameters/MathUtil.h"
#include "Generators/WavetableOscillator.h"
#include "Processors/FilterProcessor.h"


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

public:
    ElectrumVoice(EVT* tree, ModDestMap* map, int idx);

    // returns whether the voice can start a new note
    bool gateIsOn() { return gate; }
    bool isBusy();
    void startNote(int note, float velocity);
    void stopNote();
    void updateForBlock();

    int getCurrentNote() { return currentNote; }
    //called to get the current value of the given modulation source for this voice
    float getModValueForSample(const String& srcID);
    //
    float getCurrentModDestValue(const String& destID);

    //called for each sample on audio thread
    void renderNextSample (float& left, float& right);
    int getIndex() const { return index; }
};
