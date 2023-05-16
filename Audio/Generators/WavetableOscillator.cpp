#include "WavetableOscillator.h"


WavetableOscillator::WavetableOscillator(EVT* t, int idx) :
state(t),
index(idx),
baseWavetablePos(0.0f)
{
    
}

float WavetableOscillator::getNextSample(double freq, double sampleRate, float position)
{
    phase = std::fmod(phase + (float)(freq / sampleRate), 1.0f);
    return state->getOscillatorValue(index, phase, std::fmod(baseWavetablePos + position, 1.0f), freq, sampleRate);
}

void WavetableOscillator::updateBasePos()
{
    String id = IDs::oscillatorPos.toString() + String(index);
    baseWavetablePos = *state->getRawParameterValue(id);
}