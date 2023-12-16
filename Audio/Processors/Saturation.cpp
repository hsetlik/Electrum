#include "Saturation.h"
#include <cmath>

// soft clipping functions! Numbered in order of grittiness, the useful range of k is 1 to about 20
// In implementation these should also have controls for input gain and wet/dry
float Saturation::process(SaturationType type, float input, float k)
{
  switch (type)
  {
  case soft1: {
    return (2.0f / (1.0f + std::powf(MathConstants<float>::euler, -k * input))) - 1.0f;
  }
  case soft2: {
    return std::atan(k * input) / std::atan(k);
  }
  case soft3: {
    return Math::fsign(input) *
           (1.0f - std::pow(MathConstants<float>::euler, std::fabs(input * k) * -1.0f)) /
           (1.0f - std::pow(MathConstants<float>::euler, -k));
  }
  case soft4: {
    return Math::fsign(input) *
           std::powf(Math::fsign(input) * input, 1.0f / (MathConstants<float>::euler * k));
  }
  default:
    return input;
  }
}
