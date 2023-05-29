#pragma once
#include "../../Parameters/MathUtil.h"
#include "../../Parameters/DLog.h"
#include "../../Parameters/AudioSystem.h"

struct AHDSRData
{
// attack
    float attackMs;
    float attackC1Length;
    float attackC1Angle;
    float attackC2Length;
    float attackC2Angle;
// hold
    float holdMs;
// decay
    float decayMs;
    float decayC1Length;
    float decayC1Angle;
    float decayC2Length;
    float decayC2Angle;
// sustain
    float sustainLevel;
// release
    float releaseMs;
    float releaseC1Angle;
    float releaseC1Length;
    float releaseC2Length;
    float releaseC2Angle;
};

enum AHDSRPhase
{
    Attack,
    Hold,
    Decay,
    Sustain,
    Release,
    Idle
};