#pragma once

#include "Electrum/Audio/Filters/RollingRMS.h"
#include "Electrum/Audio/Modulator/AHDSR.h"
#include "Electrum/Audio/Modulator/LFO.h"
#include "Electrum/Audio/Wavetable.h"
#include "Electrum/Identifiers.h"

// the shared parameters that each of our voice filters will reference
struct shared_filter_params {
  bool active = true;
  FilterTypeE filterType = FilterTypeE::LadderLPLinear;
  float baseCutoff = 1000.0f;
  float baseResLin = 1.0f;
  float baseGainLin = 1.0f;
  bool oscActive[NUM_OSCILLATORS] = {true, true, true};
};

class CommonAudioData {
public:
  Wavetable wOsc[NUM_OSCILLATORS];
  EnvelopeLUT env[NUM_ENVELOPES];
  shared_filter_params filters[NUM_FILTERS];
  LowFrequencyLUT lfos[NUM_LFOS];
  RollingRMS polyRMS;
  CommonAudioData() = default;
};
