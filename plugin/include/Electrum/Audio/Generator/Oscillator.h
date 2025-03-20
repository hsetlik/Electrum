#pragma once
// the per-voice object that sets up our wavetable oscillators
#include "../Wavetable.h"

class WavetableOscillator {
private:
  Wavetable* const wave;
  const int index;
  float phase = 0.0f;
  float lastPositionFinal = 0.0f;

public:
  WavetableOscillator(Wavetable* w, int idx);
  float getNextSample(int midiNote,
                      float levelMod,
                      float posMod,
                      float coarseMod,
                      float fineMod);
  // equivalent to above but deals with pan as well
  void renderSampleStereo(int midiNote,
                          float levelMod,
                          float posMod,
                          float panMod,
                          float coarseMod,
                          float fineMod,
                          float& left,
                          float& right);
};
