#pragma once
#include "../../Parameters/ElectrumValueTree.h"
#include "../../Parameters/AudioSystem.h"

class AHDSREnvelope
{
private:
    EVT* const state;
    float lastOutput;
    bool gateIsOn;
    size_t samplesSinceGateChange;
public:
    const int index;
    AHDSREnvelope(EVT* tree, int idx);
    bool isFinished()
    {
        return ((!gateIsOn) && lastOutput == 0.0f);
    }
    void gateStart()
    {
        gateIsOn = true;
        samplesSinceGateChange = 0;
    }
    void gateEnd()
    {
        gateIsOn = false;
        samplesSinceGateChange = 0;
    }
    void tickSample();
    float getCurrentSample();
};
