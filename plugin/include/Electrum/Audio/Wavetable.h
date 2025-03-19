#pragma once
#include "../Common.h"
#include <juce_dsp/juce_dsp.h>

#define TABLE_SIZE 2048
#define WAVES_PER_TABLE 10

// holds a wave array with its max and min
// frequency (expressed in phase change/sample)

struct banded_wave_t {
  float maxPhaseDelt = 0.0f;
  float minPhaseDelt = 0.0f;
  float wave[TABLE_SIZE];
};

// this guy handles the band-limiting
// on initialization and is accessed
// via pointer by the rest of our
// oscillator code
class BandLimitedWave {
private:
  // the fft operates on 2^order number of
  // points, so for our 2048 point tables
  // the order is 11
  static const int fftOrder = 11;
  std::array<banded_wave_t, WAVES_PER_TABLE> data;
  juce::dsp::FFT fftProc;
  void _initTablesComplex(float* data);
  float _makeTableComplex(float* data,
                          float scale,
                          float freqLo,
                          float freqHi,
                          int tablesAdded);

public:
  BandLimitedWave(float* firstWave);
};
