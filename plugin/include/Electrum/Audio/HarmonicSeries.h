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

#define MAX_NUM_HARMONICS 127

