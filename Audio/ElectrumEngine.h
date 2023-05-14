#pragma once
#include "Modulators/Envelope.h"
#include "Modulators/Oscillator.h"
#include "../Parameters/ElectrumValueTree.h"
#include "AudioBasics.h"
#include "ElectrumVoice.h"
#define NUM_VOICES 18

class ElectrumEngine
{
private:
// state
    double sampleRate;
    int blockSize;
    OwnedArray<ElectrumVoice> voices;
    float left, right;
// functions
    void noteOn(int note, float velocity);
    void noteOff(int note);
    void renderNextSample(float& left, float& right);

    ElectrumVoice* getFreeVoice();
    ElectrumVoice* getVoicePlayingNote(int note);

    int numBusyVoices();

public:
    EVT* const state;
    ElectrumEngine(EVT* tree);
    // main callback
    void processBlock(AudioBuffer<float>& buffer, MidiBuffer& midi);
    void prepareToPlay(double newSampleRate, int newBlockSize);
    

};
