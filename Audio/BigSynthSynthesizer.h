#pragma once
#include "../Parameters/BigSynthValueTree.h"

class BigSynthSound : public SynthesiserSound
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
class BigSynthVoice : public SynthesiserVoice
{
private:
    BSVT* const state;
    const int index;
public:
    BigSynthVoice (BSVT* tree, int index);
    bool canPlaySound (SynthesiserSound* sound) override
    {
        return dynamic_cast<BigSynthSound*> (sound) != nullptr;
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
class BigSynthSynthesizer : public Synthesiser
{
private:
    BSVT* const state;
public:
    BigSynthSynthesizer (BSVT* tree);
};

