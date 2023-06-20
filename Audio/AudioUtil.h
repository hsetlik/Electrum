#pragma once
#include "../Parameters/ElectrumValueTree.h"

namespace Audio
{
    inline std::vector<size_t> getZeroCrossings(float* arr, size_t length)
    {
        std::vector<size_t> crossings;
        for(size_t i = 1; i < length; i++)
        {
            bool prevIsAbove = arr[i - 1] > 0.0f;
            bool currentIsAbove = arr[i] > 0.0f; 
            if(prevIsAbove != currentIsAbove)
                crossings.pu
        }
    }
};