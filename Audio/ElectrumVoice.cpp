#include "ElectrumVoice.h"

ElectrumVoice::ElectrumVoice (EVT* tree, int idx) : 
state (tree),
index (idx),
currentNote(-1),
currentNoteVelocity(0.0f),
gate(false)
{
    for (int i = 0; i < NUM_OSCILLATORS; i++)
    {
        oscs.add(new WavetableOscillator(state, i));
    }
}

bool ElectrumVoice::isBusy()
{
    return gate || (!env.isFinished());
}

void ElectrumVoice::startNote(int note, float vel)
{
    currentNote = note;
    currentNoteVelocity = vel;
    gate = true;
    env.gateStart();
}

void ElectrumVoice::stopNote()
{
    env.gateEnd();
    gate = false;
}

void ElectrumVoice::renderNextSample(float& left, float& right)
{
    if (!isBusy())
        return;
    static float output = 0.0f;
    output = 0.0f;
    for (auto o : oscs)
    {
        //TODO: need to calculate position mod and level mod here

        output += o->getNextSample(Math::midiToHz(currentNote), AudioSystem::getSampleRate(), 0.0f);
    }
    output = output * env.getSample() * 0.25f;
    left += output;
    right += output;
}

void ElectrumVoice::updateForBlock()
{
    for (auto o : oscs)
    {
        o->updateBasePos();
        o->updateBaseLevel();
    }
}


float ElectrumVoice::getModValueForSample(const String& srcID)
{
    if (srcID == IDs::modWheelSource.toString())
    {
        return state->getModWheel();
    }
    else
    {
        return 0.0f;
    }
}