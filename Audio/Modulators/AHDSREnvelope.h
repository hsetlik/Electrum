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
    bool inKillQuickMode;
    float killQuickDelta;
    float sampleRate;

    float prevAttackCurve;
    float prevDecayCurve;
    float prevReleaseCurve;

    float attackExp;
    float decayExp;
    float releaseExp;

    float getEnvelopeSample();
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
    void tick();
    float getCurrentSample();
    void steal();
    void killQuick();
};
