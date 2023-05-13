#include "ElectrumVoice.h"

ElectrumVoice::ElectrumVoice (EVT* tree, int idx) : 
state (tree),
index (idx),
currentNote(-1),
currentNoteVelocity(0.0f),
gateIsOn(false)
{

}

bool ElectrumVoice::isBusy()
{
    return false;
}

void ElectrumVoice::startNote(int note, float vel)
{
    currentNote = note;
    currentNoteVelocity = vel;
    gateIsOn = true;
}

void ElectrumVoice::stopNote()
{
    gateIsOn = false;
}

