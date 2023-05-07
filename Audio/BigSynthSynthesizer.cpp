#include "BigSynthSynthesizer.h"
BigSynthVoice::BigSynthVoice(BSVT* tree, int i) : state(tree), index(i)
{

}
void BigSynthVoice::startNote(int midiNoteNumber,
                              float velocity,
                              SynthesiserSound *sound,
                              int currentPitchWheelPosition)
{
}
void BigSynthVoice::stopNote(float velocity, bool allowTailOff)
{
}

void BigSynthVoice::renderNextBlock(AudioBuffer<float> &outputBuffer, int startSample, int numSamples)
{
}
//=============================================================================================================
BigSynthSynthesizer::BigSynthSynthesizer (BSVT* tree) : state(tree)
{

}