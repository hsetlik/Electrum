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

void Saturation::processStereo(float &left, float &right, SaturationType type, float coeff,
                               float drive, float wetDry)
{
  left = Math::flerp(left, process(type, left * drive, coeff) / drive, wetDry);
  right = Math::flerp(right, process(type, right * drive, coeff) / drive, wetDry);
}

//===========================================================================
SaturationProcessor::SaturationProcessor(EVT *tree) : state(tree)
{
  currentType = SaturationType::soft1;
  baseCoeff = SAT_COEFF_DEFAULT;
  baseDrive = SAT_DRIVE_DEFAULT;
  baseWetDry = 0.5f;
}

void SaturationProcessor::updateBaseParams()
{
  // grip values to stack variables
  const SaturationType t =
      (SaturationType)state->getChoiceParamValue(IDs::saturationType.toString());
  const float c = state->getFloatParamValue(IDs::saturationCoeff.toString());
  const float d = state->getFloatParamValue(IDs::saturationDrive.toString());
  const float mix = state->getFloatParamValue(IDs::saturationMix.toString());
  // assign after
  currentType = t;
  baseCoeff = c;
  baseDrive = d;
  baseWetDry = mix;
}

void SaturationProcessor::processStereo(float &left, float &right, float coeffMod, float driveMod,
                                        float mixMod)
{
  return Saturation::processStereo(
      left, right, currentType,
      Math::bipolarFlerp(SAT_COEFF_MIN, SAT_COEFF_MAX, baseCoeff, coeffMod),
      Math::bipolarFlerp(SAT_DRIVE_MIN, SAT_DRIVE_MAX, baseDrive, driveMod),
      Math::bipolarFlerp(0.0f, 1.0f, baseWetDry, mixMod));
}
