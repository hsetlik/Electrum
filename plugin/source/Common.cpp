#include "Electrum/Common.h"

static double sampleHz = 44100.0;
static float fSampleHz = 44100.0f;
static float dNyquist = 1.0f / (fSampleHz / 2.0f);

void SampleRate::set(double rate) {
  sampleHz = rate;
  fSampleHz = (float)rate;
  dNyquist = 1.0f / (fSampleHz / 2.0f);
}
double SampleRate::get() {
  return sampleHz;
}

float SampleRate::phaseDeltNyquist() {
  return dNyquist;
}
