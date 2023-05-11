#pragma once
#include "../../Core/CustomJuceHeader.h"
#define PLACEHOLDER_ENV_ATTACK 100

class PlaceholderEnvelope
{
private:
    float lastOutput;
    bool gateIsOn;
public:
    PlaceholderEnvelope() :
    lastOutput (0.0f),
    gateIsOn (false)
    {

    }
    void gateStart()
    {
        gateIsOn = true;
    }
    void gateEnd()
    {
        gateIsOn = false;
    }

};