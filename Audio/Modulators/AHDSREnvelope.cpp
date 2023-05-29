#include "AHDSREnvelope.h"

AHDSREnvelope::AHDSREnvelope(EVT* tree, int idx) : 
state(tree), 
lastOutput(0.0f),
gateIsOn(false),
samplesSinceGateChange(0),
index(idx)
{

}


float AHDSREnvelope::getNextSample()
{
    lastOutput = AHDSRData::getEnvelopeValue(state->getAudioData()->getEnvelopeData(index), gateIsOn, samplesSinceGateChange);
    ++samplesSinceGateChange;
    return lastOutput;
}