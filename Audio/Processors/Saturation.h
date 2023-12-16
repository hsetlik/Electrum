#pragma once
#include "../../Parameters/ElectrumValueTree.h"
#include "../../Parameters/MathUtil.h"
#include "../AudioUtil.h"
enum SaturationType
{
  soft1,
  soft2,
  soft3,
  soft4
};

namespace Saturation {
// the core logic of the saturation
float process(SaturationType type, float input, float coeff);
// a more useful version
void processStereo(float &l, float &r, SaturationType type, float coeff, float gain, float wetDry);

} // namespace Saturation

class SaturationProcessor
{
private:
  EVT *const state;
  SaturationType currentType;
  float baseCoeff;
  float baseDrive;
  float baseWetDry;

public:
  SaturationProcessor(EVT *tree);
  void updateBaseParams();
};
