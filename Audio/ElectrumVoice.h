#pragma once
#include "Modulators/Envelope.h"
#include "Modulators/Oscillator.h"
#include "../Parameters/ElectrumValueTree.h"
#include "../Parameters/MathUtil.h"


class ElectrumVoice
{
private:
    EVT* const state;
    const int index;
    int currentNote;
    float currentNoteVelocity;
    bool gate;
    int currentBlockSize;
    double sampleRate;
    PlaceholderEnvelope env;
    PlaceholderOsc osc;
public:
    ElectrumVoice(EVT* tree, int idx);
    // call this from prepareToPlay
    void prepareVoice(double newRate, int newBlockSize);
    // returns whether the voice can start a new note
    bool gateIsOn() { return gate; }
    bool isBusy();
    void startNote(int note, float velocity);
    void stopNote();

    int getCurrentNote() { return currentNote; }
    //called for each sample on audio thread
    void renderNextSample (float& left, float& right);
    int getIndex() const { return index; }
};