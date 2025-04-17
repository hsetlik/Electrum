#include "Electrum/Audio/Wavetable.h"
#include <algorithm>
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
    arr.add(waveStr + waveEndToken);
    tableStr = tableStr.substring(waveStr.length() + waveEndToken.length());
    tokenStartIdx = tableStr.indexOf(waveEndToken);
  }
  return arr;
}

//======================================================================
namespace Wave {
void randomizePhasesComplex(std::complex<float>* freqDomain,
                            int numBins,
                            size_t seed) {
  juce::Random rand((juce::int64)seed);
  for (int i = 1; i < numBins / 2; ++i) {
    // magnitude of each freq bin = absolute value of s-plane
    const float posMagnitude = std::abs(freqDomain[i]);
    const float negMagnitude = std::abs(freqDomain[TABLE_SIZE - i]);
    const float oldPhase = std::arg(freqDomain[i]);
    // const float negMagnitude = std::abs(freqDomain[i]);
    const float phase = rand.nextFloat() * juce::MathConstants<float>::twoPi;
    // back to s-plane via std::polar
    freqDomain[i] = std::polar(posMagnitude, flerp(oldPhase, phase, 0.05f));
  }
}

void forwardFFT(float* data) {
  static FFTProc fft(WAVE_FFT_ORDER);
  fft.performRealOnlyForwardTransform(data);
}
void inverseFFT(float* data) {
  static FFTProc fft(WAVE_FFT_ORDER);
  fft.performRealOnlyInverseTransform(data);
}
//
// static void s_analyzeGainDistribution(bin_array_t& data, float maxMagnitude)
// {
//   std::array<float, AUDIBLE_BINS> binGains = {};
//   for (size_t i = 0; i < AUDIBLE_BINS; ++i) {
//     binGains[i] = data[i].magnitude / maxMagnitude;
//   }
//   std::sort(binGains.begin(), binGains.end());
//   float medianGain = binGains[AUDIBLE_BINS / 2];
//   float medianDb = juce::Decibels::gainToDecibels(medianGain);
//   DLog::log("Medain gain is " + juce::Decibels::toString(medianDb));
// }
//
float getMedianBinMagnitude(const bin_array_t& bins) {
  std::array<float, AUDIBLE_BINS> binMags = {};
  for (size_t i = 0; i < AUDIBLE_BINS; ++i) {
    binMags[i] = bins[i].magnitude;
  }
  std::sort(binMags.begin(), binMags.end());
  return binMags[AUDIBLE_BINS / 2];
}

float getMagnitudeAtNormFrequency(const bin_array_t& bins, float freq) {
  const float fBin = freq * (float)AUDIBLE_BINS;
  const size_t bLow = AudioUtil::fastFloor64(fBin);
  const size_t bHigh = bLow + 1;
  jassert(bLow >= 0 && bHigh < AUDIBLE_BINS);
  const float t = fBin - (float)bLow;
  return flerp(bins[bLow].magnitude, bins[bHigh].magnitude, t);
}

std::vector<float> getMeanMagnitudes(const bin_array_t& bins, int window) {
  std::vector<float> vec = {};
  float currentSum = 0.0f;
  for (size_t i = 0; i < AUDIBLE_BINS; ++i) {
    currentSum += bins[i].magnitude;
    if (i % (size_t)window == 0) {
      vec.push_back(currentSum / (float)window);
      currentSum = 0.0f;
    }
  }
  return vec;
}

float loadAudibleBins(const String& wave, bin_array_t& bins, bool normalize) {
  float temp[TABLE_SIZE * 2] = {};
  float real[TABLE_SIZE];
  stringDecodeWave(wave, real);
  for (int i = 0; i < TABLE_SIZE; ++i) {
    temp[i] = real[i];
    temp[TABLE_SIZE + i] = 0.0f;
  }
  // 1. forward FFT
  forwardFFT(temp);
  auto* complex = reinterpret_cast<std::complex<float>*>(temp);
  // 2. zero out DC and Nyquist
  // complex[0] = std::complex(0.0f, 0.0f);
  complex[TABLE_SIZE >> 1] = std::complex(0.0f, 0.0f);

  // iterate over the audible half of the complex wave
  float maxMag = 1.0f;
  for (size_t i = 0; i < AUDIBLE_BINS; ++i) {
    const float mag = (float)std::abs(complex[i]);
    bins[i].magnitude = mag;
    if (maxMag < mag) {
      maxMag = mag;
    }
    bins[i].phase = std::arg(complex[i]);
  }
  // s_analyzeGainDistribution(bins, maxMag);
  if (normalize) {
    //    float gainSum = 0.0f;
    float lGain;
    for (size_t i = 0; i < AUDIBLE_BINS; ++i) {
      lGain = bins[i].magnitude / maxMag;
      // gainSum += lGain;
      bins[i].magnitude = lGain;
    }
    // const float meanGain = gainSum / (float)AUDIBLE_BINS;
    // const float meanDb = juce::Decibels::gainToDecibels(meanGain);
    // DLog::log("Mean bin gain is " + juce::Decibels::toString(meanDb));
  }
  return maxMag;
}

String audibleBinsToWaveString(freq_bin_t* bins) {
  std::complex<float> temp[TABLE_SIZE] = {};
  for (int i = 0; i < AUDIBLE_BINS; ++i) {
    temp[i] = std::polar(bins[i].magnitude, bins[i].phase);
  }
  auto* real = reinterpret_cast<float*>(temp);
  inverseFFT(real);
  return stringEncodeWave(real);
}

}  // namespace Wave
//======================================================================

static float makeBandedWave(std::complex<float>* comp,
                            float scale,
                            float loFreq,
                            float hiFreq,
                            banded_wave_t* dest) {
  dest->minPhaseDelt = loFreq;
  dest->maxPhaseDelt = hiFreq;
  // 1. cast back to float* and do the inverse FFT
  float* real = reinterpret_cast<float*>(comp);
  Wave::inverseFFT(real);
  // 2. set the scale if we haven't yet
  if (fequal(scale, 0.0f)) {
    // find the max value
    float maxVal = 0.0f;
    for (int i = 0; i < TABLE_SIZE; ++i) {
      float val = std::fabs(real[i]);
      if (val > maxVal)
        maxVal = val;
    }
    scale = 1.0f / maxVal * 0.999f;
  }
  // 3. find the max and min points of the wave
  float maxLvl = -1000.0f;
  float minLvl = 1000.0f;
  for (int i = 0; i < TABLE_SIZE; ++i) {
    real[i] *= scale;
    if (real[i] > maxLvl)
      maxLvl = real[i];
    else if (real[i] < minLvl) {
      minLvl = real[i];
    }
  }
  // 4. copy the wave to the dest pointer, adjusting for any DC offset
  const float offset = (maxLvl + minLvl) / 2.0f;
  for (int i = 0; i < TABLE_SIZE; ++i) {
    dest->wave[i] = real[i] - offset;
  }
  return scale;
}

static void initBandedWaves(std::complex<float>* bins, banded_wave_set& waves) {
  int size = TABLE_SIZE;
  static const std::complex<float> zeroBin(0.0f, 0.0f);
  // 1. Zero out the bins at DC and Nyquist
  bins[0] = zeroBin;
  bins[size >> 1] = zeroBin;
  // 2. initialize the max harmonic and
  // ignore any very quiet high frequency
  // bins
  int maxHarmonic = size >> 1;
  static const float minLvl = juce::Decibels::decibelsToGain(-100.0f);
  while (std::abs(bins[maxHarmonic]) < minLvl && maxHarmonic) {
    --maxHarmonic;
  }
  // 3. our tracking variables and temp. complex array
  std::complex<float> temp[TABLE_SIZE];
  float maxFreq = 2.0f / 3.0f / (float)maxHarmonic;
  float minFreq = 0.0f;
  float scale = 0.0f;
  size_t tables = 0;
  while (maxHarmonic && tables < WAVES_PER_TABLE) {
    // zero out the temp array before each wave
    for (int i = 0; i < TABLE_SIZE; ++i)
      temp[i] = zeroBin;
    // now copy in the appropriate harmonics
    for (int i = 1; i <= maxHarmonic; ++i) {
      // copy the values from both sides of nyquist
      temp[i] = bins[i];
      temp[size - i] = bins[size - i];
    }
    // make the band-limited wave
    scale = makeBandedWave(temp, scale, minFreq, maxFreq, &waves[tables]);
    ++tables;
    minFreq = maxFreq;
    maxFreq *= 2.0f;
    maxHarmonic >>= 1;
  }
}

BandLimitedWave::BandLimitedWave(float* firstWave) {
  // 1. allocate our temporary array and
  // put the wave values in the first half
  float dComplex[TABLE_SIZE * 2];
  for (size_t i = 0; i < TABLE_SIZE; ++i) {
    dComplex[i] = firstWave[i];
    dComplex[TABLE_SIZE + i] = 0.0f;
  }
  // 2. perform the first forward transform
  Wave::forwardFFT(dComplex);
  // 3. cast to std::complex and make the tables
  auto* bins = reinterpret_cast<std::complex<float>*>(dComplex);
  // randomizt the phases I guess
  // Wave::randomizePhasesComplex(bins);
  initBandedWaves(bins, data);
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

static std::array<float, TABLE_SIZE> getRampNormalized(float width) {
  std::array<float, TABLE_SIZE> arr;
  const size_t rampLengthSamples = (size_t)(width * (float)TABLE_SIZE);
  const float dY = 2.0f / (float)rampLengthSamples;
  const size_t rampStart = (TABLE_SIZE >> 1) - (rampLengthSamples >> 1);
  const size_t rampEnd = rampStart + rampLengthSamples;
  float value = -1.0f;
  for (size_t i = 0; i < TABLE_SIZE; ++i) {
    if (i >= rampStart && i < rampEnd) {
      value += dY;
    }
    arr[i] = value;
  }
  return arr;
}
String Wavetable::getDefaultWavesetString() {
  String str = "";
  constexpr size_t numWaves = 18;

  for (size_t t = 0; t < numWaves; ++t) {
    float width = std::max((float)t / (float)numWaves, 0.026f);
    auto arr = getRampNormalized(width);

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
String Wavetable::getDefaultSetString(int idx) {
  juce::ignoreUnused(idx);
  return getDefaultWavesetString();
}

static int numTablesCreated = 0;
Wavetable::Wavetable() {
  auto str = getDefaultSetString(numTablesCreated);
  ++numTablesCreated;

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
