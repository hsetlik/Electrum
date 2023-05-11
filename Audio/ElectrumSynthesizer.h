#pragma once
#include "../Parameters/ElectrumValueTree.h"

class ElectrumSound : public SynthesiserSound
{
public:
    bool appliesToNote (int /*midiNoteNumber*/) //just plays this sound for any midi note
    {
        return true;
    }
    bool appliesToChannel (int /*midiChannel*/) //plays the sound on both channels
    {
        return true;
    }
};
//===========================================================
class ElectrumVoice : public SynthesiserVoice
{
private:
    BSVT* const state;
    const int index;
public:
    ElectrumVoice (BSVT* tree, int index);
    bool canPlaySound (SynthesiserSound* sound) override
    {
        return dynamic_cast<ElectrumSound*> (sound) != nullptr;
    }
    void setSampleRate (double newRate, int blockSize=512)
    {
        setCurrentPlaybackSampleRate (newRate);
    }
    void startNote (int midiNoteNumber,
                    float velocity,
                    juce::SynthesiserSound *sound,
                    int currentPitchWheelPosition) override;
    void stopNote (float velocity, bool allowTailOff) override;
    void pitchWheelMoved (int newPitchWheelVal) override {}
    void controllerMoved (int controllerNumber, int controllerValue) override{}
    void aftertouchChanged (int newAftertouchValue) override{}
    void channelPressureChanged (int newChannelPressureValue) override{} 
    void renderNextBlock (juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override;

};
//===========================================================
class ElectrumSynthesizer : public Synthesiser
{
private:
    BSVT* const state;
    void renderVoices (AudioBuffer<float>& buffer, int startSample, int numSamples) override;
public:
    ElectrumSynthesizer (BSVT* tree);
};

