#include "YourPluginName/Common.h"

static double sampleHz = 44100.0;

void SampleRate::set(double rate) {
  sampleHz = rate;
}
double SampleRate::get() {
  return sampleHz;
}
