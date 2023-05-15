#include "WavetableOscillator.h"


WavetableOscillator::WavetableOscillator(EVT* t, int idx) :
state(t),
index(idx)
{
    
}

float WavetableOscillator::getNextSample(double freq, double sampleRate, float position)
{
    phase = std::fmod(phase + (float)(freq / sampleRate), 1.0f);
    return state->getOscillatorValue(index, phase, std::fmod(position, 1.0f), freq, sampleRate);
}