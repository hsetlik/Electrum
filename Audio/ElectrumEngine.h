#pragma once
#include "Modulators/Envelope.h"
#include "Modulators/Oscillator.h"
#include "../Parameters/ElectrumValueTree.h"
#include "AudioBasics.h"

class ElectrumEngine
{
private:
// state
    double sampleRate;
    int blockSize;
// functions
    void noteOn(int note, float velocity);
    void noteOff(int note);
    float getNextLeft();
    float getNextRight();
public:
    EVT* const state;
    ElectrumEngine(EVT* tree);
    // main callback
    void processBlock(AudioBuffer<float>& buffer, MidiBuffer& midi);
    void prepareToPlay(double newSampleRate, int newBlockSize);
    

};
