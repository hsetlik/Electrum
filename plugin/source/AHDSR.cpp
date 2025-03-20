#include "Electrum/Audio/Modulator/AHDSR.h"
#include "Electrum/Common.h"
float AHDSREnvelope::_getEnvelopeSample() {
  const float velGain =
      flerp(currentVelocity, 1.0f, 1.0f - params->velTracking);
  switch (currentPhase) {
    case ahdsr_phase_t::Attack: {
      if (!fequal(prevAttackCurve, params->attackCurve)) {
        prevAttackCurve = params->attackCurve;
        attackExp = std::log(prevAttackCurve) / std::log(0.5f);
      }
      const float x = phaseMs / params->attackMs;
      if (x >= 1.0f) {
        currentPhase = ahdsr_phase_t::Hold;
        phaseMs = 0.0f;
      }
      return std::powf(x, attackExp) * velGain;
      break;
    }
    case ahdsr_phase_t::Hold: {
      if (phaseMs > params->holdMs) {
        currentPhase = ahdsr_phase_t::Decay;
        phaseMs = 0.0f;
      }
      return velGain;
      break;
    }
    case ahdsr_phase_t::Decay: {
      if (!fequal(prevDecayCurve, params->decayCurve)) {
        prevDecayCurve = params->decayCurve;
        decayExp = std::log(prevDecayCurve) / std::log(0.5f);
      }
      float fNorm = phaseMs / params->decayMs;
      if (fNorm >= 1.0f) {
        currentPhase = ahdsr_phase_t::Sustain;
        phaseMs = 0.0f;
      }
      fNorm = std::powf(1.0f - fNorm, decayExp);
      return (params->sustainLevel + (fNorm * (1.0f - params->sustainLevel))) *
             velGain;
      break;
    }
    case ahdsr_phase_t::Sustain: {
      if (!gateIsOn) {
        currentPhase = ahdsr_phase_t::Release;
        phaseMs = 0.0f;
      }
      return params->sustainLevel * velGain;
      break;
    }
    case ahdsr_phase_t::Release: {
      float fNorm = phaseMs / params->releaseMs;
      if (fNorm >= 1.0f) {
        currentPhase = ahdsr_phase_t::Idle;
        phaseMs = 0.0f;
      }
      return std::powf(1.0f - fNorm, releaseExp) * params->sustainLevel *
             velGain;
      break;
    }
    case ahdsr_phase_t::Idle: {
      return 0.0f;
      break;
    }
  }
}

// helper for smooth voice stealing

static float _phaseMsForStealLevel(float lvl, ahdsr_data_t* env) {
  /*
    currentMs = samples * 1000 / sampleRate
    and t = currentMs / attackMs
    value = t^(log(midpoint)/log(0.5))
    t = value^(log(0.5) / log(midpoint))
    currentMs = attackMs * t
  */
  const float t = std::powf(lvl, std::log(0.5f) / std::log(env->attackCurve));
  return env->attackMs * t;
}

//===================================================

AHDSREnvelope::AHDSREnvelope(ahdsr_data_t* p, int idx)
    : params(p), index(idx) {}

void AHDSREnvelope::steal() {
  gateIsOn = true;
  phaseMs = _phaseMsForStealLevel(lastOutput, params);
}

void AHDSREnvelope::killQuick() {
  gateIsOn = false;
  inKillQuickMode = true;
  killQuickDelta = lastOutput / (0.0045f * SampleRate::getf());
}

void AHDSREnvelope::tick() {
  if (inKillQuickMode) {
    lastOutput -= killQuickDelta;
    inKillQuickMode = !fequal(lastOutput, 0.0f);
  } else {
    lastOutput = _getEnvelopeSample();
  }
  phaseMs += msDelta;
}
