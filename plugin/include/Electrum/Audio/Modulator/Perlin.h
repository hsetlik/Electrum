#pragma once

#include "Electrum/Common.h"
namespace Perlin {
float getNoise(float x);
float getFractal(float x, size_t octaves, float frequency, float lacunarity);
}  // namespace Perlin

class PerlinGenerator {
private:
  size_t currentOctaves;
  float currentFreq;
  float currentLacunarity;
  float currentX;
  float xDelta;
  float lastOutput = 0.0f;

  float getNextValue() {
    currentX += (currentFreq / ((float)SampleRate::get() * 6.0f));
    return Perlin::getFractal(currentX, currentOctaves, currentFreq,
                              currentLacunarity);
  }

public:
  PerlinGenerator()
      : currentOctaves(1),
        currentFreq(1.0f),
        currentLacunarity(2.0f),
        currentX(0.0f) {}
  void setParams(size_t octaves, float frequency, float lacunarity);
  void tick();
  float getValue() const { return lastOutput; }
};

