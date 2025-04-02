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

juce::StringArray splitWaveStrings(const String& input) {
  String tableStr = input;
  juce::StringArray arr;
  int tokenStartIdx = tableStr.indexOf(waveEndToken);
  while (tokenStartIdx != -1 && tableStr.length() > waveEndToken.length()) {
    String waveStr = tableStr.substring(0, tokenStartIdx);
    arr.add(waveStr);
    tableStr = tableStr.substring(waveStr.length() + waveEndToken.length());
    tokenStartIdx = tableStr.indexOf(waveEndToken);
  }
  return arr;
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

BandLimitedWave::BandLimitedWave(float* firstWave) {
  // 1. allocate our temporary arrays and initialize them
  float wReal[TABLE_SIZE];
  float wImag[TABLE_SIZE];
  for (size_t i = 0; i < TABLE_SIZE; ++i) {
    wImag[i] = firstWave[i];
    wReal[i] = 0.0f;
  }
  // 2. do the first forward FFT
  AudioUtil::wavetableFFTSplit(wReal, wImag);
  // 3. initialize the band-limited tables
  createTables(wReal, wImag);
}

void BandLimitedWave::createTables(float* real, float* imag) {
  size_t size = (int)TABLE_SIZE;
  // zero DC offset and Nyquist (set first and middle samples of each array to
  // zero, in other words)
  real[0] = imag[0] = 0.0f;
  real[size >> 1] = imag[size >> 1] = 0.0f;
  int maxHarmonic = (int)(size >> 1);
  const double minVal = 0.000001f;
  while ((fabs(real[(size_t)maxHarmonic]) + fabs(imag[(size_t)maxHarmonic]) <
          minVal) &&
         maxHarmonic) {
    --maxHarmonic;
  }

  float tReal[TABLE_SIZE] = {};
  float tImag[TABLE_SIZE] = {};
  float maxFreq = 2.0f / 3.0f / (float)maxHarmonic;
  float minFreq = 0.0f;
  float scale = 0.0f;
  size_t tables = 0;
  while (maxHarmonic && tables < WAVES_PER_TABLE) {
    // zero out both temp arrays
    for (size_t i = 0; i < TABLE_SIZE; ++i)
      tReal[i] = tImag[i] = 0.0f;
    // copy in the needed harmonics
    for (size_t i = 1; i <= (size_t)maxHarmonic; ++i) {
      tReal[i] = real[i];
      tImag[i] = imag[i];
      tReal[size - i] = real[size - i];
      tImag[size - i] = imag[size - i];
    }
    // make the table
    scale = makeTable(tReal, tImag, scale, minFreq, maxFreq, &data[tables]);
    ++tables;
    minFreq = maxFreq;
    maxFreq *= 2.0f;
    maxHarmonic >>= 1;
  }
}

float BandLimitedWave::makeTable(float* real,
                                 float* imag,
                                 float scale,
                                 float lowFreq,
                                 float hiFreq,
                                 banded_wave_t* dest) {
  dest->minPhaseDelt = lowFreq;
  dest->maxPhaseDelt = hiFreq;
  // inverse FFT to get the wave back in the time domain
  // remember: the values we care about will be in the "imag"
  // array now
  AudioUtil::wavetableFFTSplit(real, imag);
  // set the scale if needed
  if (fequal(scale, 0.0f)) {
    // get maximum value to scale to -1 - 1
    double max = 0.0f;
    for (size_t idx = 0; idx < TABLE_SIZE; idx++) {
      double temp = fabs(imag[idx]);
      if (max < temp)
        max = temp;
    }
    scale = 1.0f / (float)max * 0.999f;
  }
  // now copy the values to the dest array
  // and measure the min/max levels
  float minLvl = std::numeric_limits<float>::max();
  float maxLvl = std::numeric_limits<float>::min();
  for (size_t i = 0; i < TABLE_SIZE; ++i) {
    const float val = imag[i] * scale;
    dest->wave[i] = val;
    if (val < minLvl)
      minLvl = val;
    if (val > maxLvl)
      maxLvl = val;
  }
  // figure out the dc offset
  const float offset = maxLvl + minLvl;
  // now apply the DC offset
  for (size_t i = 0; i < TABLE_SIZE; ++i) {
    dest->wave[i] -= (offset / 2.0f);
  }
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

String Wavetable::toString() const noexcept {
  String str = "";
  for (int i = 0; i < pActive->size(); ++i) {
    str += pActive->getUnchecked(i)->toString();
  }
  return str;
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
