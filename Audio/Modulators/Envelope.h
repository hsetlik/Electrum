#pragma once
#include "../../Core/CustomJuceHeader.h"
#define PLACEHOLDER_ATTACK_MS 60
#define PLACEHOLDER_DECAY_MS 60

class PlaceholderEnvelope
{
private:
    float lastOutput;
    bool gateIsOn;
    double sampleRate;
public:
    PlaceholderEnvelope() :
    lastOutput (0.0f),
    gateIsOn (false),
    sampleRate (44100.0f)
    {

    }
    bool isFinished()
    {
        return ((!gateIsOn) && lastOutput == 0.0f);
    }
    void gateStart()
    {
        gateIsOn = true;
    }
    void gateEnd()
    {
        gateIsOn = false;
    }
    void setSampleRate (double val)
    {
        sampleRate = val;
    }
    float getSample()
    {
        static float attackDelta = 0.0f;
        static float decayDelta = 0.0f;
        if (gateIsOn)
        {
            attackDelta = 1.0f / (float)((sampleRate * (double)PLACEHOLDER_ATTACK_MS) / 1000.0f);
            lastOutput = std::min(lastOutput + attackDelta, 1.0f);
        }
        else
        {
            decayDelta = 1.0f / (float)((sampleRate * (double)PLACEHOLDER_DECAY_MS) / 1000.0f);
            lastOutput = std::max(lastOutput - decayDelta, 0.0f);
        }
        return lastOutput;
    }
};