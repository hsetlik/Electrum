#include "AHDSR.h"

AHDSRPhase AHDSRData::getCurrentPhase(AHDSRData* env, bool gateOn, size_t samplesSinceGateChange)
{
    if(gateOn)
    {
        size_t attackSamples = (size_t)((env->attackMs / 1000.0f) * (float)AudioSystem::getSampleRate());
        if (samplesSinceGateChange < attackSamples)
            return AHDSRPhase::Attack;
        size_t holdSamples = (size_t)((env->holdMs / 1000.0f) * (float)AudioSystem::getSampleRate());
        if (samplesSinceGateChange < attackSamples + holdSamples)
            return AHDSRPhase::Hold;
        size_t decaySamples = (size_t)((env->decayMs / 1000.0f) * (float)AudioSystem::getSampleRate());
        if(samplesSinceGateChange < attackSamples + holdSamples + decaySamples)
            return AHDSRPhase::Decay;
        return AHDSRPhase::Sustain;
    }
    else
    {
        size_t releaseSamples = (size_t)((env->releaseMs / 1000.0f) * (float)AudioSystem::getSampleRate());
        if (samplesSinceGateChange < releaseSamples)
            return AHDSRPhase::Release;
        return AHDSRPhase::Idle;
    }
}

float AHDSRData::getEnvelopeValue(AHDSRData* env, bool gateOn, size_t samplesSinceGateChange)
{
    auto phase = getCurrentPhase(env, gateOn, samplesSinceGateChange);
    float currentMs = (float)samplesSinceGateChange / (float)AudioSystem::getSampleRate();
    if(phase == AHDSRPhase::Attack)
    {
        float t = currentMs / env->attackMs;
        return Math::onEasingCurve(0.0f, env->attackCurve, 1.0f, t);
    }
    else if(phase == AHDSRPhase::Hold)
    {
        return 1.0f;
    }
    else if(phase == AHDSRPhase::Decay)
    {
        float t = (currentMs - env->attackMs - env->holdMs) / env->decayMs;
        float dY = 1.0f - env->sustainLevel;
        return 1.0f - Math::onEasingCurve(0.0f, env->attackCurve, dY, 1.0f - t);
    }
    else if(phase == AHDSRPhase::Sustain)
    {
        return env->sustainLevel;
    }
    else if(phase == AHDSRPhase::Release)
    {
       float t = currentMs / env->releaseMs;
       float dY = 1.0f - env->sustainLevel;
       return env->sustainLevel - Math::onEasingCurve(0.0f, env->releaseCurve, dY, 1.0f - t); 
    }
    else //idle
        return 0.0f;
}
