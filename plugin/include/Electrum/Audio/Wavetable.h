#pragma once
#include "../Common.h"
#include "juce_core/juce_core.h"
#include "juce_events/juce_events.h"
#include <juce_dsp/juce_dsp.h>

#define TABLE_SIZE 2048
#define WAVES_PER_TABLE 10
// the fft operates on 2^order number of
// points, so for our 2048 point tables
// the order is 11
#define WAVE_FFT_ORDER 11
#define ALWAYS_RANDOMIZE_PHASES

// helpers for string/wave conversion
String stringEncodeWave(float* wave);
void stringDecodeWave(const String& str, float* dest);

// holds a wave array with its max and min
// frequency (expressed in phase change/sample)

struct banded_wave_t {
  float maxPhaseDelt = 0.0f;
  float minPhaseDelt = 0.0f;
  float wave[TABLE_SIZE];
};

// transforms and utilities full-spectrum waves---------------------------
namespace Wave {
void randomizePhases(std::complex<float>* freqDomain,
                     int numBins = TABLE_SIZE,
                     size_t seed = 56392);
}
//------------------------------------------------------------------------
// this guy handles the band-limiting
// on initialization and is accessed
// via pointer by the rest of our
// oscillator code
class BandLimitedWave {
private:
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
  float getSample(float phase, float phaseDelt) const;
  String toString();
};

//=========================================================
// represents the data about a set of wavetables that our
// oscilators will access via pointer
#define MAX_WAVES_PER_TABLE 256
typedef juce::OwnedArray<BandLimitedWave> wave_set_t;
class Wavetable : public juce::AsyncUpdater {
private:
  wave_set_t setA;
  wave_set_t setB;
  wave_set_t* pActive = &setA;
  wave_set_t* pWaiting = &setB;
  float fSize;

  static String& getDefaultSetString();

  // the GUI parameters for each oscillator
  // since these are shared across voices they'll
  // be here
  float position = 0.0f;
  float level = 0.75f;
  float pan = 0.0f;
  float coarse = 0.0f;
  float fine = 0.0f;

public:
  Wavetable();
  int size() const { return pActive->size(); }
  void loadWaveData(const String& str);
  void handleAsyncUpdate() override;
  inline void setPos(float value) { position = value; }
  inline void setLevel(float value) { level = value; }
  inline void setPan(float value) { pan = value; }
  inline void setCoarse(float value) { coarse = value; }
  inline void setFine(float value) { fine = value; }
  //  these do the main work for the oscillators
  float getSampleFixed(float phase, float phaseDelt, float pos) const;
  float getSampleSmooth(float phase, float phaseDelt, float pos) const;
  // parameter getters for the oscillator code
  inline float getPos() const { return position; }
  inline float getLevel() const { return level; }
  inline float getPan() const { return pan; }
  inline float getCoarse() const { return coarse; }
  inline float getFine() const { return fine; }
};
