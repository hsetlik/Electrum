#include "WavetableOscillator.h"


WavetableOscillator::WavetableOscillator(EVT* t, int idx) :
state(t),
index(idx),
baseWavetablePos(0.0f),
baseLevel(1.0f)
{
    
}

float WavetableOscillator::getNextSample(double freq, double sampleRate, float position)
{
    phase = std::fmod(phase + (float)(freq / sampleRate), 1.0f);
    return state->getOscillatorValue(index, phase, std::fmod(baseWavetablePos + position, 1.0f), freq, sampleRate) * baseLevel;
}

void WavetableOscillator::updateBasePos()
{
    String id = IDs::oscillatorPos.toString() + String(index);
    baseWavetablePos = *state->getAPVTS()->getRawParameterValue(id);
}

void WavetableOscillator::updateBaseLevel()
{
    String id = IDs::oscillatorLevel.toString() + String(index);
    baseLevel = *state->getAPVTS()->getRawParameterValue(id);
}