#pragma once
#include "../AudioUtil.h"

#define RMS_SIZE 512

#define RMS_TO_PEAK 1.414

class RollingRMS {
private:
  double sum = 0.0;
  std::array<float, RMS_SIZE> data = {};
  size_t head = 0;
  double lastMean = 0.0;

public:
  RollingRMS() = default;
  void clear() { std::fill(data.begin(), data.end(), 0.0f); }
  void tick(float left, float right);
  float currentLevel() const;
};
