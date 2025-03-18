#pragma once
/* This file just includes juce and sets up a few
 * typedefs that I find handy in every juce project
 * */
#include "Identifiers.h"
#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_gui_basics/juce_gui_basics.h"
constexpr float twoPi_f = juce::MathConstants<float>::twoPi;

// some type aliases to maybe type juce:: fewer times
typedef juce::String String;
typedef juce::Component Component;
typedef juce::ValueTree ValueTree;
// maybe we'll avoid writing 5 zillion 'prepare()' functions this way
namespace SampleRate {
void set(double rate);
double get();
}  // namespace SampleRate

inline float flerp(float a, float b, float t) {
  return a + ((b - a) * t);
}
