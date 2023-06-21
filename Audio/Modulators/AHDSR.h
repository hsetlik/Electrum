#pragma once
#include "../../Parameters/MathUtil.h"
#include "../../Parameters/DLog.h"
#include "../../Parameters/AudioSystem.h"


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
    float attackCurve = 0.5f;
// hold
    float holdMs = 0.0f;
// decay
    float decayMs = DECAY_MS_DEFAULT;

// sustain
    float sustainLevel = SUSTAIN_LEVEL_DEFAULT;
// release
    float releaseMs = RELEASE_MS_DEFAULT;

// static methods to hande the logic of turning these parameters into an envelope output
    static AHDSRPhase getCurrentPhase(AHDSRData* env, bool gateOn, size_t samplesSinceGateChange);
// this does the heavy lifting
    static float getEnvelopeValue(AHDSRData* env, bool gateOn, size_t samplesSinceGateChange);
};

