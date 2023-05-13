#pragma once
#include "Modulators/Envelope.h"
#include "Modulators/Oscillator.h"
#include "../Parameters/ElectrumValueTree.h"

class ElectrumVoice
{
private:
    EVT* const state;
    const int index;
    int currentNote;
    float currentNoteVelocity;
    bool gateIsOn;
public:
    ElectrumVoice(EVT* tree, int idx);
    // returns whether the voice can start a new note
    bool isBusy();
    void startNote(int note, float velocity);
    void stopNote();
    



};