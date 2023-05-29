#pragma once
#include "Identifiers.h"
#include "../Audio/Generators/Wavetable.h"
#include "../Audio/Modulators/AHDSR.h"
class ElectrumAudioData
{
private:
    OwnedArray<WavetableSet> oscillators;
    OwnedArray<AHDSRData> envData;
public:
    // default constructor
    ElectrumAudioData ();
    // for loading from a valueTree object
    ElectrumAudioData (ValueTree& state);
    float getOscillatorValue(int idx, float phase, float tablePos, double freq, double sampleRate);
    // get the data for a given envelope
    AHDSRData* getEnvelopeData(int index) { return envData[index]; }

    




};
