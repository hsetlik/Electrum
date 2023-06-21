#pragma once
#include "Modulators/Envelope.h"
#include "Modulators/Oscillator.h"
#include "../Parameters/ElectrumValueTree.h"
#include "ElectrumVoice.h"
#include <stack>
#define NUM_VOICES 18

class ElectrumEngine
{
private:
    struct TimestampedMidiMessage
    {
        int timestamp;
        MidiMessage message;
    };
    std::queue<TimestampedMidiMessage> midiQueue;
    ModDestMap currentModulation;
// state
    OwnedArray<ElectrumVoice> voices;
    float left, right;
    
// functions
    void noteOn(int note, float velocity);
    void noteOff(int note);
    void renderNextSample(float& left, float& right);

    ElectrumVoice* getFreeVoice();
    ElectrumVoice* getVoicePlayingNote(int note);

    int numBusyVoices();
    void updateParamsForBlock();
//helpers for processBlock
    void loadMidiEvents(MidiBuffer& midi);
    void handleMidiMessage(MidiMessage& message);

public:
    EVT* const state;
    ElectrumEngine(EVT* tree);
    // main callback
    void processBlock(AudioBuffer<float>& buffer, MidiBuffer& midi);
};
