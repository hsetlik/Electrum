#pragma once
#include "../../Core/CustomJuceHeader.h"

class PlaceholderOsc
{
private:
    double sampleRate;
    float phase;
public:
    PlaceholderOsc() : sampleRate(44100.0f), phase (0.0f)
    {

    }
    void setSampleRate(double newRate)
    {
        sampleRate = newRate;
    }

    float getNextSample (double fundamental)
    {
        phase += (float)(fundamental / sampleRate);
        phase = std::fmod(phase, 1.0f);
        return std::sin(phase * juce::MathConstants<float>::twoPi);
    }

};