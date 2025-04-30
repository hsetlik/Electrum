#include "Electrum/Audio/Filters/RollingRMS.h"

void RollingRMS::tick(float left, float right) {
  const float avg = std::fabs((left + right) / 2.0f);
  // 1. subtract the oldest value from the sum
  sum -= (double)data[head];
  // 2. add the new value
  sum += (double)avg;
  // 3. update the mean
  lastMean = sum / (double)RMS_SIZE;
  // 4. update the ring buffer
  data[head] = avg;
  head = (head + 1) % RMS_SIZE;
}

float RollingRMS::currentLevel() const {
  return (float)(lastMean * RMS_TO_PEAK);
}
//===================================================
