#pragma once
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
float process(SaturationType type, float input, float coeff);
}
