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
    float curveX = 1.0f - (phase / peakPhase);
    return std::pow(curveX, t);
  }
}
