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
    }
    return AHDSRPhase::Idle;
}

AHDSRPhase AHDSRData::getPhaseForMs(AHDSRData* env, bool gateOn, float msSinceGateChange)
{
  if(gateOn)
  {
    float current = env->attackMs;
    if(msSinceGateChange < current)
      return AHDSRPhase::Attack;
    current += env->holdMs;
    if(msSinceGateChange < current)
      return AHDSRPhase::Hold;
    current += env->decayMs;
    if(msSinceGateChange < current)
      return AHDSRPhase::Decay;
    return AHDSRPhase::Sustain;
  }
  else if(msSinceGateChange < env->releaseMs)
      return AHDSRPhase::Release;
  return AHDSRPhase::Idle;
}

float AHDSRData::getEnvelopeValue(AHDSRData* env, bool gateOn, size_t samplesSinceGateChange)
{
    auto phase = getCurrentPhase(env, gateOn, samplesSinceGateChange);
    if(phase == AHDSRPhase::Idle)
      return 0.0f;
    float currentMs = (float)samplesSinceGateChange * (float)(1000.0f / AudioSystem::getSampleRate());
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
        float t = (currentMs - (env->attackMs + env->holdMs)) / env->decayMs;
        float dY = 1.0f - env->sustainLevel;
        return env->sustainLevel + Math::onEasingCurve(0.0f, env->decayCurve * dY, dY, 1.0f - t);
    }
    else if(phase == AHDSRPhase::Sustain)
    {
        return env->sustainLevel;
    }
    else if(phase == AHDSRPhase::Release)
    {
       float t = currentMs / env->releaseMs;
       return Math::onEasingCurve(0.0f, env->releaseCurve * env->sustainLevel, env->sustainLevel, 1.0f - t); 
    }
    else 
        return 0.0f;
}

size_t AHDSRData::getSamplesForAttackValue(AHDSRData* env, float value)
{
  /*
    currentMs = samples * 1000 / sampleRate
    and t = currentMs / attackMs
    value = t^(log(midpoint)/log(0.5))
    t = value^(log(0.5) / log(midpoint))
    currentMs = attackMs * t
    samples = currentMs / (1000 / sampleRate)
  */
  float t = std::pow(value, std::log(0.5f) / std::log(env->attackCurve));
  float currentMs = t * env->attackMs;
  float samples = currentMs / (1000.0f / (float)AudioSystem::getSampleRate());
  return (size_t)samples;
}

