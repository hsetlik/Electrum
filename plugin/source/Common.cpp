#include "Electrum/Common.h"

namespace DLog {

void log(const String& str) {
#if JUCE_DEBUG
  std::cout << str.toStdString() << "\n";
#endif
}
}  // namespace DLog
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

float SampleRate::getf() {
  return fSampleHz;
}

float SampleRate::phaseDeltNyquist() {
  return dNyquist;
}
