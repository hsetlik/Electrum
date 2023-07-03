#include "ElectrumEngine.h"

ElectrumEngine::ElectrumEngine (EVT* tree) : 
left (0.0f),
right (0.0f),
state (tree)
{
    for (int i = 0; i < NUM_VOICES; i++)
    {
        voices.add (new ElectrumVoice(state, &currentModulation, i));
    }

}

void ElectrumEngine::noteOn(int note, float velocity)
{
    auto voice = getFreeVoice();
    jassert(voice != nullptr);
    voice->startNote(note, velocity);
    //auto busy = numBusyVoices();
   // String str = (busy == 1) ? " voice is busy" : " voices are busy";
    //DLog::log(String(busy) + str);
}

void ElectrumEngine::noteOff(int note)
{
    
    auto voice = getVoicePlayingNote(note);
    if(voice != nullptr)
    {
        voice->stopNote();
    }
    else
    {
        DLog::log("No voice found for note: " + String(note));
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
        if (v->getCurrentNote() == note && v->gateIsOn())
            return v;
    }
    return nullptr;
}
void ElectrumEngine::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midi)
{
    TRACE_DSP();
    state->loadModulationData(currentModulation);
    updateParamsForBlock();
    loadMidiEvents(midi);
    //make sure we have stereo
    jassert(buffer.getNumChannels() >= 2);
    for (int s = 0; s < buffer.getNumSamples(); s++)
    {
        //STEP 1: Check if we have a midi event on this sample
        while(!midiQueue.empty() && midiQueue.front().timestamp == s)
        {
            handleMidiMessage(midiQueue.front().message);
            midiQueue.pop();
        }

        //STEP 3: Render the actual samples
        renderNextSample(left, right);
        buffer.setSample(0, s, left);
        buffer.setSample(1, s, right);
    }
    //ensure that the midi queue is empty (i.e. no out of range timestamps)
    jassert(midiQueue.empty());
}


void ElectrumEngine::renderNextSample(float& l, float& r)
{
   
    state->tickPerlinForSample();
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


void ElectrumEngine::updateParamsForBlock()
{
    state->updatePerlinForBlock();
    state->updateEnvelopesForBlock();
    for (auto v : voices)
        v->updateForBlock();

}

void ElectrumEngine::loadMidiEvents(MidiBuffer& midi)
{
    for(auto it = midi.begin(); it != midi.end(); ++it)
    {
        auto metadata = *it;
        TimestampedMidiMessage m;
        m.timestamp = metadata.samplePosition;
        m.message = metadata.getMessage();
        midiQueue.push(m);
    }

}
void ElectrumEngine::handleMidiMessage(MidiMessage& message)
{
    // big ol else if to handle every type of MIDI message
    if (message.isNoteOn())
    {
        noteOn(message.getNoteNumber(), message.getVelocity());
    }
    else if(message.isNoteOff())
    {
        noteOff(message.getNoteNumber());
    }
    else if(message.isSustainPedalOn())
    {
        state->setSustainPedal(true);
    }
    else if(message.isSustainPedalOff())
    {
        state->setSustainPedal(false);
    }
    else if(message.isController() && message.getControllerNumber() == 1) // handle mod wheel
    {
        float modVal = (float)message.getControllerValue() / 127.0f;
        state->setModWheel(modVal);
    }
    else if(message.isPitchWheel())
    {
        state->setPitchBend(Math::toPitchBendValue(message.getPitchWheelValue()));
    }
    else
    {
        DLog::log("Warning! Unhandled MIDI message: " + message.getDescription());
    }

}
//=============================================================
