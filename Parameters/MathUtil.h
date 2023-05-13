#pragma once
#include "../Core/CustomJuceHeader.h"
#define SEMITONE_RATIO 1.05946309436f
namespace Math
{
    inline float flerp (float a, float b, float t)
    {
        return a + ((b - a) * t);
    }

    inline double dlerp (double a, double b, double t)
    {
        return a + ((b - a) * t);
    }
    inline double midiToHz(int midiNum)
    {
        return 440.0f * std::pow (SEMITONE_RATIO, (float)midiNum - 69);
    }
}