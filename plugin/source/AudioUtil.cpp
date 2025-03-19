#include "Electrum/Audio/AudioUtil.h"
#include <cmath>
#include "Electrum/Common.h"

namespace AudioUtil {
int fastFloor32(float fValue) {
  return 32768 - (int)(32768. - fValue);
}
size_t fastFloor64(float fValue) {
  return (size_t)fValue;
}

#define FAST_SINE_POINTS 2048
static std::array<float, FAST_SINE_POINTS> generateSine() {
  std::array<float, FAST_SINE_POINTS> arr;
  const float dPhase = twoPi_f / (float)FAST_SINE_POINTS;
  float phase = 0.0f;
  for (size_t i = 0; i < FAST_SINE_POINTS; ++i) {
    arr[i] = std::sinf(phase);
    phase += dPhase;
  }
  return arr;
}

static std::array<float, FAST_SINE_POINTS> sinePoints = generateSine();
float fastSine(float phaseNorm) {
  return sinePoints[fastFloor64(phaseNorm * 2048.0f)];
}

}  // namespace AudioUtil
//===================================================
