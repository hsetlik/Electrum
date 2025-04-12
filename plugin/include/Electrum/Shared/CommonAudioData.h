#pragma once

#include "Electrum/Audio/Modulator/AHDSR.h"
#include "Electrum/Audio/Wavetable.h"
#include "Electrum/Identifiers.h"
// the shared parameters that each of our voice filters will reference
struct shared_filter_params {
  bool active = true;
  float baseCutoff = 1000.0f;
  float baseResLin = 1.0f;
  float baseGainLin = 1.0f;
};

class CommonAudioData {
public:
  Wavetable wOsc[NUM_OSCILLATORS];
  EnvelopeLUT env[NUM_ENVELOPES];
  shared_filter_params filters[NUM_FILTERS];
  CommonAudioData() = default;
};
