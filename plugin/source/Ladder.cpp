#include "Electrum/Audio/Filters/Ladder.h"
#include "juce_core/juce_core.h"

void TPTLadder::setCutoffHz(float hz) {
  if (!fequal(hz, cutoffHz)) {
    cutoffHz = hz;
    updateG(SampleRate::get());
  }
}

void TPTLadder::updateG(double sampleRate) {
  g = (float)std::tan(juce::MathConstants<double>::pi * (double)cutoffHz /
                      sampleRate);

  g2 = g * g;
  g3 = g2 * g;
  g4 = g3 * g;
  bigG = g / (1.0f + g);
}

TPTLadder::TPTLadder() {
  for (int i = 0; i < 4; ++i) {
    zState[0][i] = 0.0f;
    zState[1][i] = 0.0f;
  }
}

float TPTLadder::processMono(float input, int channel) {
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

void TPTLadder::processStereo(float& left, float& right) {
  left = processMono(left, 0);
  right = processMono(right, 1);
}
//===================================================
