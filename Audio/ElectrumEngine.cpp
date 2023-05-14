#include "ElectrumEngine.h"

ElectrumEngine::ElectrumEngine (EVT* tree) : 
sampleRate (44100.0f),
blockSize(512),
left (0.0f),
right (0.0f),
state (tree)
{
    for (int i = 0; i < NUM_VOICES; i++)
    {
        voices.add (new ElectrumVoice (state, i));
    }

}

void ElectrumEngine::noteOn(int note, float velocity)
{
    auto voice = getFreeVoice();
    jassert(voice != nullptr);
    std::cout << "Starting note " << note << " on voice " << voice->getIndex() << "\n";
    voice->startNote(note, velocity);
}

void ElectrumEngine::noteOff(int note)
{
    
    auto voice = getVoicePlayingNote(note);
    if(voice != nullptr)
    {
        voice->stopNote();
        std::cout << "Stopping note " << note << " on voice " << voice->getIndex() << "\n";
    }
    else
    {
        std::cout << "No voices found playing note " << note << "\n";
    }
}

ElectrumVoice* ElectrumEngine::getFreeVoice()
{
    for (auto v : voices)
    {
        if (!v->isBusy())
            return v;
    }
    return nullptr;
}

ElectrumVoice* ElectrumEngine::getVoicePlayingNote(int note)
{
    //ElectrumVoice* voice = nullptr;
    for (auto v : voices)
    {
        if (v->getCurrentNote() == note)
            return v;
    }
    return nullptr;
}
void ElectrumEngine::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midi)
{
    TRACE_DSP();
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

                // make sure we increment the iterator after processing each message
                it++;
            }
        }
        //STEP 2: Render the actual samples
        renderNextSample(left, right);
        buffer.setSample(0, s, left);
        buffer.setSample(1, s, right);
    }
}

void ElectrumEngine::prepareToPlay(double newSampleRate, int newBlockSize)
{
    sampleRate = newSampleRate;
    blockSize = newBlockSize;
    for (int v = 0; v < voices.size(); v++)
    {
        voices[v]->prepareVoice(sampleRate, blockSize);
    }
}

void ElectrumEngine::renderNextSample(float& l, float& r)
{
    l = 0.0f;
    r = 0.0f;
    for (auto v : voices)
    {
        v->renderNextSample (l, r);
    }
}

int ElectrumEngine::numBusyVoices()
{
    int count = 0;
    for (auto v : voices)
    {
        if (v->isBusy())
            ++count;
    }
    return count;
}
//=============================================================