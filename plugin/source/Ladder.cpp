#include "Electrum/Audio/Filters/Ladder.h"
#include "juce_core/juce_core.h"

void TPTLadderLinear::setCutoffHz(float hz) {
  if (!fequal(hz, cutoffHz)) {
    cutoffHz = hz;
    updateG(SampleRate::get());
  }
}

void TPTLadderLinear::updateG(double sampleRate) {
  g = (float)std::tan(juce::MathConstants<double>::pi * (double)cutoffHz /
                      sampleRate);

  g2 = g * g;
  g3 = g2 * g;
  g4 = g3 * g;
  bigG = g / (1.0f + g);
}

TPTLadderLinear::TPTLadderLinear() {
  for (int i = 0; i < 4; ++i) {
    zState[0][i] = 0.0f;
    zState[1][i] = 0.0f;
  }
}

float TPTLadderLinear::processMono(float input, int channel) {
  // this is the math described on p 63 of the Zavalishin book
  // 1. find S
  auto S = (g3 * zState[channel][0]) + (g2 * zState[channel][1]) +
           (g * zState[channel][2]) + zState[channel][3];
  // 2. now we can find U (input of the low pass series)
  float x = (input - k * S) / (1.0f + k * g4);
  // 3. now we process each filter
  float v;
  for (int i = 0; i < 4; ++i) {
    auto& s = zState[channel][i];
    v = (x - s) * bigG;
    x = v + s;
    zState[channel][i] = x + v;
  }
  return x;
}

void TPTLadderLinear::processStereo(float& left, float& right) {
  left = processMono(left, 0);
  right = processMono(right, 1);
}
//===================================================

TPTLadderNonLinear::TPTLadderNonLinear() {
  for (int i = 0; i < 4; ++i) {
    zState[0][i] = 0.0f;
    zState[1][i] = 0.0f;
  }
}

void TPTLadderNonLinear::updateG(double sampleRate) {
  g = (float)std::tan(juce::MathConstants<double>::pi * (double)cutoffHz /
                      sampleRate);

  g2 = g * g;
  g3 = g2 * g;
  g4 = g3 * g;
  bigG = g / (1.0f + g);
}

float TPTLadderNonLinear::processMono(float input, int channel) {
  auto S = (g3 * zState[channel][0]) + (g2 * zState[channel][1]) +
           (g * zState[channel][2]) + zState[channel][3];
  // 2. now we can find U (input of the low pass series)
  float u = (input - k * S) / (1.0f + k * g4);
  // 2.5 put u through a tanh function as a
  // "cheap" means of applying saturation (p. 73 in the Zavalishin book)
  u = std::tanhf(u);
  // 3. now we process each filter
  float v, s;
  for (int i = 0; i < 4; ++i) {
    s = zState[channel][i];
    v = (u - s) * bigG;
    u = v + s;
    zState[channel][i] = u + v;
  }
  return u;
}

void TPTLadderNonLinear::processStereo(float& left, float& right) {
  left = processMono(left, 0);
  right = processMono(right, 1);
}

void TPTLadderNonLinear::setCutoffHz(float hz) {
  if (!fequal(hz, cutoffHz)) {
    cutoffHz = hz;
    updateG(SampleRate::get());
  }
}
