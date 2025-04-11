#pragma once

#include "Electrum/Audio/Modulator/AHDSR.h"
#include "Electrum/Audio/Wavetable.h"
class CommonAudioData {
public:
  Wavetable wOsc[NUM_OSCILLATORS];
  EnvelopeLUT env[NUM_ENVELOPES];
  CommonAudioData() = default;
};
