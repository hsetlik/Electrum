#include "ElectrumEngine.h"

ElectrumEngine::ElectrumEngine (EVT* tree) : 
sampleRate (44100.0f),
blockSize(512),
state (tree)
{

}

void ElectrumEngine::noteOn(int note, float velocity)
{

}

void ElectrumEngine::noteOff(int note)
{

}
float ElectrumEngine::getNextLeft()
{
    return 0.0f;
}
float ElectrumEngine::getNextRight()
{
    return 0.0f;
}

void ElectrumEngine::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midi)
{
    MidiBufferIterator it = midi.begin();
    //make sure we have stereo
    jassert(buffer.getNumChannels() >= 2);
    for (int s = 0; s < buffer.getNumSamples(); s++)
    {
        //STEP 1: Check if we have a midi event on this sample
        if (it != midi.end())
        {
            auto metadata = *it;
            if (metadata.samplePosition == s)
            {
                auto message = metadata.getMessage();
                if (message.isNoteOn())
                {
                    noteOn(message.getNoteNumber(), message.getFloatVelocity());
                }
                else if (message.isNoteOff())
                {
                    noteOff(message.getNoteNumber());
                }
                //other else ifs can handle other types of midi message as needed

                // make sure we increment the iterator after processing the message
                it++;
            }
        }
        //STEP 2: Render the actual samples
        buffer.setSample(0, s, getNextLeft());
        buffer.setSample(1, s, getNextRight());
    }
}

void ElectrumEngine::prepareToPlay(double newSampleRate, int newBlockSize)
{
    sampleRate = newSampleRate;
    blockSize = newBlockSize;
}
//=============================================================