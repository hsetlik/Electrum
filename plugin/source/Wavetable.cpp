#include "Electrum/Audio/Wavetable.h"
#include <limits>

BandLimitedWave::BandLimitedWave(float* firstWave) : fftProc(fftOrder) {
  // we need an array of 2X table size to hold the complex output
  // of the FFT
  float complex[2 * TABLE_SIZE];
  // load the wave into the first half
  for (int i = 0; i < TABLE_SIZE; ++i) {
    complex[i] = firstWave[i];
  }
  // do the first freq domain transformation
  fftProc.performRealOnlyForwardTransform(complex);
  // and the helpers bo the band-limiting work
  _initTablesComplex(complex);
}

// helper for accessing interleaved complex arrays
void BandLimitedWave::_initTablesComplex(float* raw) {
  // the juce FFT function interleaves this such that we can just
  // cast straight to std::complex and make life easier
  std::complex<float>* complex = reinterpret_cast<std::complex<float>*>(raw);
  // zero the bins at DC and nyquist
  complex[0] = 0.0f;
  complex[TABLE_SIZE >> 1] = 0.0f;
  constexpr float minMagnitude = 0.00000001f;
  // ignore any other high frequency bins
  // with very small amplitudes
  int maxHarmonic = TABLE_SIZE >> 1;
  while (std::abs(complex[maxHarmonic]) < minMagnitude && maxHarmonic) {
    --maxHarmonic;
  }
  // allocate a new array
  std::array<std::complex<float>, TABLE_SIZE> temp = {};
  float topFreq = (float)(2.0f / 3.0f / (float)maxHarmonic);
  int tablesAdded = 0;
  float lastMinFreq = 0.0f;
  float scale = 0.0f;
  // and go through each of the tables
  while (maxHarmonic > 0) {
    // 1. zero out the temp array from the last pass
    for (auto& d : temp) {
      d = std::complex(0.0f, 0.0f);
    }
    // 2. fill the temp array with the needed harmonics from the input
    for (size_t i = 0; i < (size_t)maxHarmonic; ++i) {
      temp[i] = complex[i];
      // the same # of upper harmonics from the end of the input get copied
      temp[TABLE_SIZE - (i + 1)] = complex[TABLE_SIZE - (i + 1)];
    }
    // 3. make the wavetable
    float* tPtr = reinterpret_cast<float*>(temp.data());
    scale = _makeTableComplex(tPtr, scale, lastMinFreq, topFreq, tablesAdded);
    ++tablesAdded;
    lastMinFreq = topFreq;
    topFreq *= 2.0f;
    maxHarmonic >>= 1;
  }
}

float BandLimitedWave::_makeTableComplex(float* raw,
                                         float scale,
                                         float freqLo,
                                         float freqHi,
                                         int tablesAdded) {
  data[(size_t)tablesAdded].maxPhaseDelt = freqHi;
  data[(size_t)tablesAdded].minPhaseDelt = freqLo;
  // inverse FFT back to time domain
  fftProc.performRealOnlyInverseTransform(raw);
  if (fequal(scale, 0.0f)) {
    float maxMag = 0.0f;
    for (int i = 0; i < TABLE_SIZE; ++i) {
      maxMag = std::max(std::fabs(raw[i]), maxMag);
    }
    scale = 1.0f / (maxMag * 0.000009f);
  }

  float currentMin = std::numeric_limits<float>::max();
  float currentMax = std::numeric_limits<float>::min();
  // copy the wave into place and scale it as appropriate
  // find the max and min levels for calculating the next round's
  // scale
  size_t table = (size_t)tablesAdded;
  for (size_t i = 0; i < TABLE_SIZE; ++i) {
    data[table].wave[i] = raw[i] * scale;
    currentMin = std::min(currentMin, data[table].wave[i]);
    currentMax = std::max(currentMax, data[table].wave[i]);
  }
  const float offset = (currentMax + currentMin) / 2.0f;
  // loop over the last wave again to correct any dc offset
  for (size_t i = 0; i < TABLE_SIZE; ++i)
    data[table].wave[i] -= offset;
  return scale;
}
//===================================================
