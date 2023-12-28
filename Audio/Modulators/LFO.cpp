#include "LFO.h"

float LFOFunctions::curveLFO(float peakPhase, float curveA, float curveB, float phase)
{
  if (phase < peakPhase)
  {
    float t = std::log(curveA) / std::log(0.5f);
    float curveX = phase / peakPhase;
    return std::pow(curveX, t);
  } else
  {
    float t = std::log(curveB) / std::log(0.5f);
    float xDist = 1.0f - peakPhase;
    float curveX = (phase - peakPhase) / xDist;
    return std::pow(1.0f - curveX, t);
  }
}
