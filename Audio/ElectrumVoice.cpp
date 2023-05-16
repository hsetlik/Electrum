#include "ElectrumVoice.h"

ElectrumVoice::ElectrumVoice (EVT* tree, int idx) : 
state (tree),
index (idx),
currentNote(-1),
currentNoteVelocity(0.0f),
gate(false),
currentBlockSize(512),
sampleRate(44100.0f)
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

void ElectrumVoice::prepareVoice(double newRate, int newBlockSize)
{
    sampleRate = newRate;
    currentBlockSize = newBlockSize;
}
void ElectrumVoice::renderNextSample(float& left, float& right)
{
    if (!isBusy())
        return;
    static float output = 0.0f;
    output = 0.0f;
    for (auto o : oscs)
    {
        //TODO: once modulation is set up, we need calculate the instantaneour wavetable position here
        output += o->getNextSample(Math::midiToHz(currentNote), sampleRate, 0.4f);
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
    }
}