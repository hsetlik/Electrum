#include "AHDSR.h"
#include "AHDSREnvelope.h"

AHDSREnvelope::AHDSREnvelope(EVT *tree, int idx)
    : state(tree), lastOutput(0.0f), gateIsOn(false), samplesSinceGateChange(0),
      inKillQuickMode(false), killQuickDelta(0.0f), index(idx)
{
  sampleRate = (float)AudioSystem::getSampleRate();
}

float AHDSREnvelope::getCurrentSample() { return lastOutput; }

void AHDSREnvelope::tick()
{
  if (inKillQuickMode)
  {
    lastOutput = std::max(lastOutput - killQuickDelta, 0.0f);
    inKillQuickMode = lastOutput != 0.0f;
  } else
    lastOutput = getEnvelopeSample();
  ++samplesSinceGateChange;
}

void AHDSREnvelope::steal()
{
  gateIsOn = true;
  // because the attack curve goes all the way from 0 to 1, we can calculate a
  // value of samplesSinceGateChange that will place this envelope at the point
  // on the attack curve that matches the current value of lastOutput
  auto env = state->getAudioData()->getEnvelopeData(index);
  samplesSinceGateChange = AHDSRData::getSamplesForAttackValue(env, lastOutput);
}

void AHDSREnvelope::killQuick()
{
  gateIsOn = false;
  inKillQuickMode = true;
  killQuickDelta = lastOutput / ((QUICK_KILL_MS / 1000.0f) * sampleRate);
}

float AHDSREnvelope::getEnvelopeSample()
{
  auto *env = state->getAudioData()->getEnvelopeData(index);
  float currentMs = (float)samplesSinceGateChange * (float)(1000.0f / sampleRate);
  auto currentPhase = AHDSRData::getPhaseForMs(env, gateIsOn, currentMs);
  float modLevel = Math::flerp(currentVelocity, 1.0f, 1.0f - env->velTracking);
  switch (currentPhase)
  {
  case AHDSRPhase::Attack: {
    if (!fequal(prevAttackCurve, env->attackCurve))
    {
      prevAttackCurve = env->attackCurve;
      attackExp = std::log(prevAttackCurve) / std::log(0.5f);
    }
    float x = currentMs / env->attackMs;
    return std::pow(x, attackExp) * modLevel;
    break;
  }
  case AHDSRPhase::Hold:
    return modLevel;
    break;
  case AHDSRPhase::Decay: {
    float fX = (currentMs - (env->attackMs + env->holdMs)) / env->decayMs;
    if (!fequal(prevDecayCurve, env->decayCurve))
    {
      prevDecayCurve = env->decayCurve;
      decayExp = std::log(prevDecayCurve) / std::log(0.5f);
    }
    float normY = std::pow(1.0f - fX, decayExp);
    return (env->sustainLevel + (normY * (1.0f - env->sustainLevel))) * modLevel;
    break;
  }
  case AHDSRPhase::Sustain:
    return env->sustainLevel * modLevel;
    break;
  case AHDSRPhase::Release: {
    float fX = currentMs / env->releaseMs;
    if (!fequal(prevReleaseCurve, env->releaseCurve))
    {
      prevReleaseCurve = env->releaseCurve;
      releaseExp = std::log(prevReleaseCurve) / std::log(0.5f);
    }
    return std::pow(1.0f - fX, releaseExp) * env->sustainLevel * modLevel;
    break;
  }
  case AHDSRPhase::Idle:
    return 0.0f;
    break;
  default:
    return 0.0f;
    break;
  }
  return 0.0f;
}
