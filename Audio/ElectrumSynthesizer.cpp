#include "ElectrumSynthesizer.h"
ElectrumVoice::ElectrumVoice(BSVT* tree, int i) : state(tree), index(i)
{

}
void ElectrumVoice::startNote(int midiNoteNumber,
                              float velocity,
                              SynthesiserSound *sound,
                              int currentPitchWheelPosition)
{
}
void ElectrumVoice::stopNote(float velocity, bool allowTailOff)
{
}

void ElectrumVoice::renderNextBlock(AudioBuffer<float> &buffer, int startSample, int numSamples)
{

}
//=============================================================================================================
ElectrumSynthesizer::ElectrumSynthesizer (BSVT* tree) : state(tree)
{

}

void ElectrumSynthesizer::renderVoices (AudioBuffer<float>& buffer, int startSample, int numSamples)
{
    const ScopedLock  sl(lock);
    for(auto v : voices)
    {
        v->renderNextBlock(buffer, startSample, numSamples);
    }
}