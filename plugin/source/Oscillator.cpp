#include "Electrum/Audio/Generator/Oscillator.h"
#include "Electrum/Audio/AudioUtil.h"
#include "Electrum/Identifiers.h"
#include "juce_audio_basics/juce_audio_basics.h"
#include "juce_audio_processors/juce_audio_processors.h"

WavetableOscillator::WavetableOscillator(Wavetable* w, int idx)
    : wave(w), index(idx) {}
//===================================================
//
float WavetableOscillator::getNextSample(int midiNote,
                                         float levelMod,
                                         float posMod,
                                         float coarseMod,
                                         float fineMod) {
  static const float minLvl = juce::Decibels::decibelsToGain(-50.0f);
  if (wave->getLevel() + levelMod < minLvl)
    return 0.0f;
  const float _coarse = AudioUtil::signed_flerp(
      COARSE_TUNE_MIN, COARSE_TUNE_MAX, wave->getCoarse(), coarseMod);
  const float _fine = AudioUtil::signed_flerp(FINE_TUNE_MIN, FINE_TUNE_MAX,
                                              wave->getFine(), fineMod);
  const float _pos =
      AudioUtil::signed_flerp(0.0f, 1.0f, wave->getPos(), posMod);
}
