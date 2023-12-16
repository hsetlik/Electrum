#pragma once
#include "../Parameters/ElectrumValueTree.h"

namespace Audio {
inline std::vector<size_t> getZeroCrossings(float *arr, size_t length, size_t offset = 0)
{
  std::vector<size_t> crossings = {};
  for (size_t i = 1; i < length; i++)
  {
    bool prevIsAbove = arr[i - 1] > 0.0f;
    bool currentIsAbove = arr[i] > 0.0f;
    if (prevIsAbove != currentIsAbove)
      crossings.push_back(i + offset);
  }
  return crossings;
}
inline std::vector<size_t> getZeroCrossings(AudioBuffer<float> &buf, size_t offset = 0,
                                            size_t channelsToUse = 1)
{
  std::vector<size_t> crossings;
  for (int s = 1; s < buf.getNumSamples(); s++)
  {
    for (int c = 0; c < channelsToUse; c++)
    {
      bool prevIsAbove = buf.getSample(c, s - 1) > 0.0f;
      bool currentIsAbove = buf.getSample(c, s) > 0.0f;
      if (prevIsAbove != currentIsAbove)
        crossings.push_back((size_t)s + offset);
    }
  }
  return crossings;
}
} // namespace Audio
