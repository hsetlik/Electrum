#pragma once
#include "../../Parameters/AudioSystem.h"
#include "../../Parameters/DLog.h"
#include "../../Parameters/MathUtil.h"

namespace LFOFunctions {
// this does the actual logic of the LFO output
float curveLFO(float peakPhase, float curveA, float curveB, float phase);
} // namespace LFOFunctions
struct LFOData
{
  float frequency;
  float center;
  float curveA;
  float curveB;
};
