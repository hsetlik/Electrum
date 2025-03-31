#include "Electrum/Audio/Wavetable.h"
#include <limits>
#include "Electrum/Audio/AudioUtil.h"
#include "Electrum/Common.h"
#include "juce_audio_basics/juce_audio_basics.h"
#include "juce_core/juce_core.h"

// stuff for encoding/decoding wavetables as strings

constexpr size_t tableBytes = TABLE_SIZE * sizeof(float);
static const String waveEndToken = "WAVE_END";
// get the next wave's worth of stringified data from a
// string containing multiple waves
String stringEncodeWave(float* wave) {
  juce::MemoryBlock mb(tableBytes, true);
  mb.copyFrom(wave, 0, tableBytes);
  return mb.toBase64Encoding() + waveEndToken;
}

void stringDecodeWave(const String& in, float* dest) {
  juce::MemoryBlock mb(tableBytes, true);
  String str = in;
  // if the string still contains the end token,
  // trim it now
  int endTokenPos = str.indexOf(waveEndToken);
  if (endTokenPos != -1) {
    str = str.substring(0, endTokenPos);
  }
  if (!mb.fromBase64Encoding(str)) {
    DLog::log("Failed to decode string to memory block!");
  }
  jassert(mb.getSize() == tableBytes);
  mb.copyTo(dest, 0, tableBytes);
}

//======================================================================
namespace Wave {
void randomizePhases(std::complex<float>* freqDomain,
                     int numBins,
                     size_t seed) {
  juce::Random rand((juce::int64)seed);
  for (int i = 0; i < numBins; ++i) {
    // magnitude of each freq bin = absolute value of s-plane
    const float magnitude = std::abs(freqDomain[i]);
    const float phase = rand.nextFloat() * juce::MathConstants<float>::twoPi;
    // back to s-plane via std::polar
    freqDomain[i] = std::polar(magnitude, phase);
  }
}
}  // namespace Wave
//======================================================================

// band-limiting helpers-----------------------------
static float _getFFTBinMagnitude(float* data, int binIdx) {
  jassert(binIdx < TABLE_SIZE);
  const float real = data[2 * binIdx];
  const float imag = data[(2 * binIdx) + 1];
  return std::abs(std::complex<float>(real, imag));
}

static void _setFFTBinMagnitude(float* data, int binIdx, float magnitude) {
  jassert(binIdx < TABLE_SIZE);
  const float real = data[2 * binIdx];
  const float imag = data[(2 * binIdx) + 1];
  std::complex<float> vComp(real, imag);
  const float phaseAngle = std::arg(vComp);
  vComp = std::polar(magnitude, phaseAngle);
  data[2 * binIdx] = vComp.real();
  data[2 * binIdx + 1] = vComp.imag();
}

static void _copyFFTBin(float* srcData,
                        float* destData,
                        int binIdx,
                        bool copyConjugate = false) {
  // 1. copy the below-nyquist parts
  const int rIdx = 2 * binIdx;
  const int iIdx = 2 * binIdx + 1;
  destData[rIdx] = srcData[rIdx];
  destData[iIdx] = srcData[iIdx];
  const int endIdx = (2 * TABLE_SIZE) - 1;
  if (copyConjugate) {
    destData[endIdx - rIdx] = srcData[endIdx - rIdx];
    destData[endIdx - iIdx] = srcData[endIdx - iIdx];
  }
}

static float _initBandedWave(float* dComplex,
                             FFTProc& fft,
                             banded_wave_t* bw,
                             float minFreq,
                             float maxfreq,
                             float scale) {
  // 1. set the min/max freq as appropriate
  bw->maxPhaseDelt = maxfreq;
  bw->minPhaseDelt = minFreq;
  // 2. perform the inverse FFT to get our real wave
  fft.performRealOnlyInverseTransform(dComplex);
  // 3. figure out the scale if it's still set to zero
  if (fequal(scale, 0.0f)) {
    float maxMag = 0.0f;
    for (int i = 0; i < TABLE_SIZE; ++i) {
      float mag = std::fabs(dComplex[i]);
      if (maxMag < mag)
        maxMag = mag;
    }
    scale = 1.0f / (maxMag * 0.999f);
  }
  // 4. iterate over the wave to measure its min/max values to compensate for dc
  // offset
  float minLvl = std::numeric_limits<float>::max();
  float maxLvl = std::numeric_limits<float>::min();
  for (int i = 0; i < TABLE_SIZE; ++i) {
    // also multiply by the scale here
    dComplex[i] *= scale;
    const float val = dComplex[i];
    if (val < minLvl)
      minLvl = val;
    if (val > maxLvl)
      maxLvl = val;
  }
  const float gain = 2.0f / (maxLvl - minLvl);
  // 5. iterate over the input again to
  // correct for any DC offset and copy to the output
  const float dc = (maxLvl + minLvl) / 2.0f;
  minLvl = std::numeric_limits<float>::max();
  maxLvl = std::numeric_limits<float>::min();
  for (int i = 0; i < TABLE_SIZE; ++i) {
    float val = (dComplex[i] - dc) * gain;
    bw->wave[i] = val;
    if (val < minLvl)
      minLvl = val;
    if (val > maxLvl)
      maxLvl = val;
  }
  jassert(maxLvl - minLvl <= 2.01f);
  return scale;
}

constexpr size_t COMPLEX_SIZE = TABLE_SIZE * 2;

static FFTProc forwardProc(WAVE_FFT_ORDER);
static FFTProc inverseProc(WAVE_FFT_ORDER);
BandLimitedWave::BandLimitedWave(float* firstWave) {
  // 1. prepare a complex array for the first FFT
  float dComplex[COMPLEX_SIZE];
  for (int i = 0; i < TABLE_SIZE; ++i) {
    jassert(std::fabs(firstWave[i]) <= 1.0f);
    dComplex[i] = firstWave[i];
    dComplex[TABLE_SIZE + i] = 0.0f;
  }
  // 2. create the FFT objects we'll use for the rest of this
  jassert(forwardProc.getSize() == TABLE_SIZE);
  // 3. do the first forward transform
  forwardProc.performRealOnlyForwardTransform(dComplex);
  const int tSize = TABLE_SIZE;
  // 4. zero out the bins at 0hZ (dc offset)
  // and nyquist
  _setFFTBinMagnitude(dComplex, 0, 0.0f);
  _setFFTBinMagnitude(dComplex, tSize / 2, 0.0f);
  // 5. find the max harmonic, starting from nyquist
  // and ignoring any other high frequency
  // bins w very low magnitude
  static const float minMagnitude = juce::Decibels::decibelsToGain(-100.0f);
  int maxHarmonic = tSize >> 1;
  while (_getFFTBinMagnitude(dComplex, maxHarmonic) < minMagnitude &&
         maxHarmonic) {
    --maxHarmonic;
  }
  // 6. create the band-limted tables
  float maxPhaseDelt = 2.0f / 3.0f / (float)maxHarmonic;
  float minPhaseDelt = 0.0f;
  std::array<float, COMPLEX_SIZE> tempComplex;
  float scale = 0.0f;
  size_t tables = 0;
  while (maxHarmonic && tables < WAVES_PER_TABLE) {
    // 1. clear the temp array
    std::fill(tempComplex.begin(), tempComplex.end(), 0.0f);
    float* td = tempComplex.data();
    // 2. copy the bins up to the max harmonic
    for (int idx = 1; idx <= maxHarmonic; ++idx) {
      _copyFFTBin(dComplex, td, idx);
    }
    // 3. convert back to time domain and init our wave object
    scale = _initBandedWave(td, inverseProc, &data[tables], minPhaseDelt,
                            maxPhaseDelt, scale);
    // 4. increment/decrement stuff
    ++tables;
    minPhaseDelt = maxPhaseDelt;
    maxPhaseDelt *= 2.0f;
    maxHarmonic = (maxHarmonic >> 1);
  }
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
  const size_t rampStart = (TABLE_SIZE / 2) - (rampWidth / 2);
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
  float tempWave[TABLE_SIZE];
  int endTokenPos = str.indexOf(waveEndToken);
  while (endTokenPos != -1 && str.length() > 0) {
    String waveStr = str.substring(0, endTokenPos);
    stringDecodeWave(waveStr, tempWave);
    arr->add(new BandLimitedWave(tempWave));
    // trim the string
    const int newStart = waveStr.length() + waveEndToken.length();
    str = str.substring(newStart);
    endTokenPos = str.indexOf(waveEndToken);
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
  DLog::log("Initialized " + String(pActive->size()) + " wave shapes");
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

std::vector<float> Wavetable::normVectorForWave(int wave, int numPoints) const {
  bool firstNegValue = false;
  std::vector<float> vec = {};
  const float freq = 20.0f / SampleRate::getf();
  float phase;
  for (int i = 0; i < numPoints; ++i) {
    phase = (float)i / (float)numPoints;
    float x = pActive->getUnchecked(wave)->getSample(phase, freq);

    const float value = (x + 1.0f) / 2.0f;

    jassert(value >= 0.0f && value <= 1.0f);
    if (value < 0.0f && !firstNegValue) {
      DLog::log("Error: got negative norm value of " + String(value) +
                " for wave input " + String(wave));
      firstNegValue = true;
    }
    vec.push_back(value);
  }
  return vec;
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
