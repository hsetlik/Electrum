#include "Electrum/Audio/Filters/TPTFilter.h"
#include "juce_core/juce_core.h"

TPTFilter::TPTFilter() {
  updateG(44100.0);
  z1[0] = 0.0f;
  z1[1] = 0.0f;
}

float TPTFilter::processMono(float input, int channel) {
  auto& z = z1[channel];
  auto v = bigG * (input - z);
  auto y = v + z;
  z = y + v;

  switch (type) {
    case TPTType::lowpass:
      return y;
    case TPTType::highpass:
      return input - y;
    case TPTType::allpass:
      return (y * 2.0f) - input;
    default:
      break;
  }
  jassert(false);
  return input;
}

void TPTFilter::processStereo(float& left, float& right) {
  left = processMono(left, 0);
  right = processMono(right, 1);
}

void TPTFilter::setFrequency(float hz) {
  if (!fequal(hz, cutoffHz)) {
    cutoffHz = hz;
    updateG(SampleRate::get());
  }
}

void TPTFilter::setType(TPTType t) {
  if (t != type) {
    type = t;
    updateG(SampleRate::get());
  }
}

void TPTFilter::updateG(double sampleRate) {
  auto g = (float)std::tan(juce::MathConstants<double>::pi * (double)cutoffHz /
                           sampleRate);
  bigG = g / (1.0f + g);
}

