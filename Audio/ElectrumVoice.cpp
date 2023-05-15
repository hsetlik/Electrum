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
    float output = osc.getNextSample(Math::midiToHz(currentNote)) * env.getSample() * 0.25f; //this .3 is temporary gain limiting...
    left += output;
    right += output;
}