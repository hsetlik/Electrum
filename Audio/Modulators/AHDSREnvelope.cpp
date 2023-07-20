#include "AHDSREnvelope.h"

AHDSREnvelope::AHDSREnvelope(EVT* tree, int idx) : 
state(tree), 
lastOutput(0.0f),
gateIsOn(false),
samplesSinceGateChange(0),
inKillQuickMode(false),
killQuickDelta(0.0f),
index(idx)
{

}

float AHDSREnvelope::getCurrentSample()
{
    return lastOutput;
}

void AHDSREnvelope::tick()
{
  if(inKillQuickMode)
  {
      lastOutput = std::max(lastOutput - killQuickDelta, 0.0f);
      inKillQuickMode = lastOutput != 0.0f;
  }
  else
    lastOutput = AHDSRData::getEnvelopeValue(state->getAudioData()->getEnvelopeData(index), gateIsOn, samplesSinceGateChange);
  ++samplesSinceGateChange;
}

void AHDSREnvelope::steal()
{
  gateIsOn = true;
  // because the attack curve goes all the way from 0 to 1, we can calculate a value of samplesSinceGateChange
  // that will place this envelope at the point on the attack curve that matches the current value of lastOutput
  auto env = state->getAudioData()->getEnvelopeData(index);
  samplesSinceGateChange = AHDSRData::getSamplesForAttackValue(env, lastOutput);

}

void AHDSREnvelope::killQuick()
{
  gateIsOn = false;
  inKillQuickMode = true;
  killQuickDelta = lastOutput / ((QUICK_KILL_MS / 1000.0f) * (float)AudioSystem::getSampleRate());
}
