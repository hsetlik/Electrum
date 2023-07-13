#include "AHDSREnvelope.h"

AHDSREnvelope::AHDSREnvelope(EVT* tree, int idx) : 
state(tree), 
lastOutput(0.0f),
gateIsOn(false),
samplesSinceGateChange(0),
index(idx)
{

}

float AHDSREnvelope::getCurrentSample()
{
    return lastOutput;
}

void AHDSREnvelope::tickSample()
{
    lastOutput = AHDSRData::getEnvelopeValue(state->getAudioData()->getEnvelopeData(index), gateIsOn, samplesSinceGateChange);
    ++samplesSinceGateChange;
}
