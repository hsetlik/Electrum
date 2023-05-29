#include "WavetableOscillator.h"


WavetableOscillator::WavetableOscillator(EVT* t, int idx) :
state(t),
index(idx),
baseWavetablePos(0.0f),
baseLevel(1.0f)
{
    
}

float WavetableOscillator::getNextSample(double freq, double sampleRate, float position, float level)
{
    if (baseLevel == 0.0f)
        return 0.0f;
    phase = std::fmod(phase + (float)(freq / sampleRate), 1.0f);
    return state->getOscillatorValue(index, phase, std::fmod(baseWavetablePos + position, 1.0f), freq, sampleRate) * std::fmod(baseLevel + level, 1.0f);
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