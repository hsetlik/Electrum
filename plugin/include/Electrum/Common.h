#pragma once
/* This file just includes juce and sets up a few
 * typedefs that I find handy in every juce project
 * */
#include <limits>
#include "Identifiers.h"
#include "Shared/ElectrumState.h"
#include "juce_audio_basics/juce_audio_basics.h"
#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_gui_basics/juce_gui_basics.h"
constexpr float twoPi_f = juce::MathConstants<float>::twoPi;

namespace DLog {
void log(const String& str);
}

// maybe we'll avoid writing 5 zillion 'prepare()' functions this way
namespace SampleRate {
void set(double rate);
double get();
float getf();
float phaseDeltNyquist();
}  // namespace SampleRate

inline float flerp(float a, float b, float t) {
  return a + ((b - a) * t);
}

inline bool fequal(float a,
                   float b,
                   float epsilon = std::numeric_limits<float>::epsilon()) {
  return std::fabs(a - b) < epsilon;
}
