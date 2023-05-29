#include "AHDSR.h"

AHDSRPhase AHDSRData::getCurrentPhase(AHDSRData* env, bool gateOn, size_t samplesSinceGateChange)
{
    if(gateOn)
    {
        size_t attackSamples = (size_t)(env->attackMs / 1000.0f) * (float)AudioSystem::getSampleRate();
        if (samplesSinceGateChange < attackSamples)
            return AHDSRPhase::Attack;
        size_t holdSamples = (size_t)(env->holdMs / 1000.0f) * (float)AudioSystem::getSampleRate();
        if (samplesSinceGateChange < attackSamples + holdSamples)
            return AHDSRPhase::Hold;
        size_t decaySamples = (size_t)(env->decayMs / 1000.0f) * (float)AudioSystem::getSampleRate();
        if(samplesSinceGateChange < attackSamples + holdSamples + decaySamples)
            return AHDSRPhase::Decay;
        return AHDSRPhase::Sustain;
    }
    else
    {
        size_t releaseSamples = (size_t)(env->releaseMs / 1000.0f) * (float)AudioSystem::getSampleRate();
        if (samplesSinceGateChange < releaseSamples)
            return AHDSRPhase::Release;
        return AHDSRPhase::Idle;
    }
}

float AHDSRData::getEnvelopeValue(AHDSRData* env, bool gateOn, size_t samplesSinceGateChange)
{
    auto phase = getCurrentPhase(env, gateOn, samplesSinceGateChange);
    float currentMs = (float)samplesSinceGateChange / (float)AudioSystem::getSampleRate();
    switch(phase)
    {
        case AHDSRPhase::Attack:
            return Math::flerp(0.0f, 1.0f, (currentMs / env->releaseMs));
        case AHDSRPhase::Hold:
            return 1.0f;
        case AHDSRPhase::Decay:
            return Math::flerp(1.0f, env->sustainLevel, (currentMs - (env->attackMs + env->holdMs)) / env->decayMs);
        case AHDSRPhase::Sustain:
            return env->sustainLevel;
        case AHDSRPhase::Release:
            return Math::flerp(env->sustainLevel, 0.0f, currentMs / env->releaseMs);
        case AHDSRPhase::Idle:
            return 0.0f;
        default:
            return 0.0f;
    }
}
