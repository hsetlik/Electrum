#include "Electrum/Audio/Wavetable.h"
#include <limits>
#include "Electrum/Audio/AudioUtil.h"
#include "Electrum/Common.h"
#include "juce_core/juce_core.h"

constexpr size_t tableBytes = TABLE_SIZE * sizeof(float);
String stringEncodeWave(float* wave) {
  juce::MemoryBlock mb(tableBytes, true);
  mb.copyFrom(wave, 0, tableBytes);
  return mb.toBase64Encoding() + "WAVE_END";
}
void stringDecodeWave(const String& str, float* dest) {
  juce::MemoryBlock mb(tableBytes, true);
  if (!mb.fromBase64Encoding(str)) {
    DLog::log("Failed to decode string to memory block!");
  }
  mb.copyTo(dest, 0, tableBytes);
}

//======================================================================

BandLimitedWave::BandLimitedWave(float* firstWave) : fftProc(fftOrder) {
  // we need an array of 2X table size to hold the complex output
  // of the FFT
  float complex[2 * TABLE_SIZE];
  // load the wave into the first half
  for (int i = 0; i < TABLE_SIZE; ++i) {
    complex[i] = firstWave[i];
    complex[TABLE_SIZE + i] = 0.0f;
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

float BandLimitedWave::getSample(float phase, float phaseDelt) const {
  size_t iWave = 0;
  while (data[iWave].maxPhaseDelt < phaseDelt && iWave < WAVES_PER_TABLE) {
    ++iWave;
  }
  size_t iIdx = AudioUtil::fastFloor64(phase * 2048.0f);
  return data[iWave].wave[iIdx];
}

String BandLimitedWave::toString() {
  return stringEncodeWave(data[0].wave);
}
//===================================================

// some basic wave shape generation for the default
// waves
static std::array<float, TABLE_SIZE> genTriangleWave() {
  std::array<float, TABLE_SIZE> arr;
  const float dY = 2.0f / ((float)TABLE_SIZE / 2.0f);
  float val = -1.0f;
  for (size_t i = 0; i < TABLE_SIZE; ++i) {
    if (i < (TABLE_SIZE / 2)) {
      val += dY;
    } else {
      val -= dY;
    }
    arr[i] = val;
  }
  return arr;
}

static std::array<float, TABLE_SIZE> genSineWave() {
  std::array<float, TABLE_SIZE> arr;
  constexpr float dPhase =
      juce::MathConstants<float>::twoPi / (float)TABLE_SIZE;
  for (size_t i = 0; i < TABLE_SIZE; ++i) {
    arr[i] = std::sinf(dPhase * (float)i);
  }
  return arr;
}

static std::array<float, TABLE_SIZE> getSawWave() {
  std::array<float, TABLE_SIZE> arr;
  const float dY = 2.0f / (float)TABLE_SIZE;

  for (size_t i = 0; i < TABLE_SIZE; ++i) {
    arr[i] = -1.0f + (dY * (float)i);
  }
  return arr;
}

static std::array<float, TABLE_SIZE> getNarrowRamp(size_t rampWidth) {
  std::array<float, TABLE_SIZE> arr;

  const float dY = 2.0f / (float)rampWidth;
  const size_t rampStart = (TABLE_SIZE >> 1) - (rampWidth >> 1);
  const size_t rampEnd = rampStart + rampWidth;
  for (size_t i = 0; i < TABLE_SIZE; ++i) {
    if (i < rampStart) {
      arr[i] = -1.0f;
    } else if (i < rampEnd) {
      arr[i] = -1.0f + (dY * (float)(i - rampStart));
    } else {
      arr[i] = 1.0f;
    }
  }
  return arr;
}

static String getDefaultWavesetString() {
  String str = "";
  constexpr size_t numWaves = 18;
  constexpr size_t rampMax = (size_t)(0.625f * (float)TABLE_SIZE);
  constexpr size_t dRamp = 56;
  for (size_t t = 0; t < numWaves; ++t) {
    auto arr = getNarrowRamp(rampMax - (dRamp * t));
    str += stringEncodeWave(arr.data());
  }
  return str;
}

static void loadWavesetIntoArr(wave_set_t* arr, const String& input) {
  if (!arr->isEmpty())
    arr->clear();
  String str = input;
  float wave[TABLE_SIZE] = {};
  int endTagPos = str.indexOf("WAVE_END");
  while (endTagPos != -1 && arr->size() < MAX_WAVES_PER_TABLE) {
    String waveStr = str.substring(0, endTagPos);
    str = str.substring(waveStr.length() + 8);
    stringDecodeWave(waveStr, wave);
    arr->add(new BandLimitedWave(wave));
    endTagPos = str.indexOf("WAVE_END");
  }
}

//====================================================================
String& Wavetable::getDefaultSetString() {
  static String str = getDefaultWavesetString();
  return str;
}

Wavetable::Wavetable() {
  auto str = getDefaultSetString();

  loadWavesetIntoArr(pActive, str);
  fSize = (float)(pActive->size() - 1);
}

// this should be thread-safe because it loads
// data into the array at the 'waiting' pointer
// but the audio thread will only touch the 'active'
// pointer
void Wavetable::loadWaveData(const String& str) {
  loadWavesetIntoArr(pWaiting, str);
  triggerAsyncUpdate();
}

// and this is where we swap the pointers
// for the audio thread
void Wavetable::handleAsyncUpdate() {
  wave_set_t* prevActive = pActive;
  pActive = pWaiting;
  pWaiting = prevActive;
  fSize = (float)(pActive->size() - 1);
}

float Wavetable::getSampleFixed(float phase, float phaseDelt, float pos) const {
  int idx = AudioUtil::fastFloor32(pos * fSize);
  return pActive->getUnchecked(idx)->getSample(phase, phaseDelt);
}

float Wavetable::getSampleSmooth(float phase,
                                 float phaseDelt,
                                 float pos) const {
  float temp = pos * fSize;
  const int lIdx = AudioUtil::fastFloor32(temp);
  const int hIdx = lIdx + 1;
  temp -= (float)lIdx;
  return flerp(pActive->getUnchecked(lIdx)->getSample(phase, phaseDelt),
               pActive->getUnchecked(hIdx)->getSample(phase, phaseDelt), temp);
}
