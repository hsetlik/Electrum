#pragma once
#include "../Audio/Generators/Wavetable.h"
#include "../Audio/Modulators/AHDSR.h"
#include "../Audio/Modulators/LFO.h"
#include "Identifiers.h"
class ElectrumAudioData
{
private:
  OwnedArray<WavetableSet> oscillators;
  OwnedArray<AHDSRData> envData;
  OwnedArray<LFOData> lfoData;

public:
  // default constructor
  ElectrumAudioData();
  // for loading from a valueTree object
  ElectrumAudioData(ValueTree &state);
  float getOscillatorValue(int idx, float phase, float tablePos, double freq, double sampleRate);
  // get the data for a given envelope
  AHDSRData *getEnvelopeData(int index) { return envData[index]; }
  std::vector<Wave> getBaseWaves(int idx) { return oscillators[idx]->getBaseWaves(); }
  LFOData *getLFOData(int index) { return lfoData[index]; }
};
