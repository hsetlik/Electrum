#include "WavetableOscillator.h"


WavetableOscillator::WavetableOscillator(EVT* t, int idx) :
state(t),
index(idx),
baseWavetablePos(0.0f),
baseLevel(1.0f),
levelMod(0.0f),
posMod(0.0f)
{
    
}

float WavetableOscillator::getNextSample(double freq, double sampleRate, float position, float levelMod)
{
    if (baseLevel + levelMod <= 0.0f)
        return 0.0f;
    phase = std::fmod(phase + (float)(freq / sampleRate), 1.0f);
    return state->getOscillatorValue(index, phase, Math::bipolarFlerp(0.0f, 1.0f, baseWavetablePos, position), freq, sampleRate) * Math::bipolarFlerp(0.0f, 1.0f, baseLevel, levelMod);
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
