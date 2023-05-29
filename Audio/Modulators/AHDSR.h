#pragma once
#include "../../Parameters/MathUtil.h"
#include "../../Parameters/DLog.h"
#include "../../Parameters/AudioSystem.h"
#define BEZIER_POINTS 512


enum AHDSRPhase
{
    Attack,
    Hold,
    Decay,
    Sustain,
    Release,
    Idle
};

struct AHDSRData
{
// attack
    float attackMs = ATTACK_MS_DEFAULT;
    float attackC1Length = CONTROL_LENGTH_DEFAULT;
    float attackC1Angle;
    float attackC2Length = CONTROL_LENGTH_DEFAULT;
    float attackC2Angle;
// hold
    float holdMs = 0.0f;
// decay
    float decayMs = DECAY_MS_DEFAULT;
    float decayC1Length = CONTROL_LENGTH_DEFAULT;
    float decayC1Angle;
    float decayC2Length = CONTROL_LENGTH_DEFAULT;
    float decayC2Angle;
// sustain
    float sustainLevel = SUSTAIN_LEVEL_DEFAULT;
// release
    float releaseMs = RELEASE_MS_DEFAULT;
    float releaseC1Angle;
    float releaseC1Length = CONTROL_LENGTH_DEFAULT;
    float releaseC2Angle;
    float releaseC2Length = CONTROL_LENGTH_DEFAULT;
// static methods to hande the logic of turning these parameters into an envelope output
    static AHDSRPhase getCurrentPhase(AHDSRData* env, bool gateOn, size_t samplesSinceGateChange);
// this does the heavy lifting
    static float getEnvelopeValue(AHDSRData* env, bool gateOn, size_t samplesSinceGateChange);
};

