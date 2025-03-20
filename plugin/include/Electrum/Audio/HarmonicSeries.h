#pragma once
#include "Wavetable.h"

/* Represents a sine wave harmonic's
 * relationship to a fundamental frequency,
 * the idea being that we can build a complex
 * wave shape from a chain of these with only the
 * note's fundamental frequency.
 * */

struct harmonic_t {
  float ratio =
      1.0f;  // the linear ratio of this harmonic's frequency to the fundamental
  float phaseOffset = 0.0f;
  float amplitude = 1.0f;
};

/*
#define MAX_NUM_HARMONICS 256
class HarmonicSeries {
private:
  harmonic_t harmonics[MAX_NUM_HARMONICS];
  int usedHarmonics = 0;

public:
  HarmonicSeries();
  void loadData(const String& data);
  String toString();
  int getNumHarmonics() const { return usedHarmonics; }
  float getSample(float phase, float fundamental) const;
};
*/
