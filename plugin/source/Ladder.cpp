#include "Electrum/Audio/Filters/Ladder.h"

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
  bigG = g3 * g;
  bigGFilter = g / (1.0f + g);
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
  auto s = (g3 * zState[channel][0]) + (g2 * zState[channel][1]) +
           (g * zState[channel][2]) + zState[channel][3];
  // 2. now we can find U (input of the low pass series)
  auto y = (input - k * s) / (1.0f + k * bigG);
  // 3. now we process each filter
  float v;
  for (int i = 0; i < 4; ++i) {
    auto& z = zState[channel][i];
    v = bigGFilter * (y - z);
    y = v + z;
    z = y + v;
  }
  return y;
}

void TPTLadder::processStereo(float& left, float& right) {
  left = processMono(left, 0);
  right = processMono(right, 1);
}
//===================================================
