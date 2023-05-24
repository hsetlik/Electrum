#pragma once
#include "Identifiers.h"
#include "../Audio/Generators/Wavetable.h"
#include "../Audio/Modulators/Perlin.h"
class ElectrumAudioData
{
private:
    OwnedArray<WavetableSet> oscillators;
public:
    // default constructor
    ElectrumAudioData ();
    // for loading from a valueTree object
    ElectrumAudioData (ValueTree& state);
    float getOscillatorValue(int idx, float phase, float tablePos, double freq, double sampleRate);

    




};
