#include "Electrum/Audio/Modulator/AHDSR.h"
#include "Electrum/Common.h"

void EnvelopeLUT::setAttackMs(float value) {
  if (!fequal(data.attackMs, value)) {
    data.attackMs = value;
    triggerAsyncUpdate();
  }
}

void EnvelopeLUT::setAttackCurve(float value) {
  if (!fequal(data.attackCurve, value)) {
    data.attackCurve = value;
    triggerAsyncUpdate();
  }
}

void EnvelopeLUT::setHoldMs(float value) {
  if (!fequal(data.holdMs, value)) {
    data.holdMs = value;
    // this is simple enough that we'll just
    // calculate it here rather than in an
    // async update
    holdLengthSamples = (int)(SampleRate::get() * (double)(value / 1000.0));
  }
}

void EnvelopeLUT::setDecayMs(float value) {
  if (!fequal(data.decayMs, value)) {
    data.decayMs = value;
    triggerAsyncUpdate();
  }
}

void EnvelopeLUT::setDecayCurve(float value) {
  if (!fequal(data.decayCurve, value)) {
    data.decayCurve = value;
    triggerAsyncUpdate();
  }
}

void EnvelopeLUT::setSustainLevel(float value) {
  if (!fequal(data.sustainLevel, value)) {
    data.sustainLevel = value;
    triggerAsyncUpdate();
  }
}

void EnvelopeLUT::setReleaseMs(float value) {
  if (!fequal(data.releaseMs, value)) {
    data.releaseMs = value;
    triggerAsyncUpdate();
  }
}

void EnvelopeLUT::setReleaseCurve(float value) {
  if (!fequal(data.releaseCurve, value)) {
    data.releaseCurve = value;
    triggerAsyncUpdate();
  }
}

EnvelopeLUT::EnvelopeLUT() {
  _computeLUTs();
}
//=======================================================================
void EnvelopeLUT::_computeLUTs() {
  // 1. calculate the attack LUT
  const float attackExp = std::log(data.attackCurve) / std::log(0.5f);
  atkCurveLength = (int)(SampleRate::get() * (double)(data.attackMs / 1000.0f));
  float dX = 1.0f / (float)atkCurveLength;
  float x = 0.0f;
  for (int i = 0; i < atkCurveLength; ++i) {
    atkCurve[i] = std::powf(x, attackExp);
    x += dX;
  }
  // 2. calculate the decay LUT
  const float decayExp = std::log(data.decayCurve) / std::log(0.5f);
  decayCurveLength =
      (int)(SampleRate::get() * (double)(data.decayMs / 1000.0f));
  dX = 1.0f / (float)decayCurveLength;
  x = 1.0f;
  const float decayHeight = 1.0f - data.sustainLevel;
  for (int i = 0; i < decayCurveLength; ++i) {
    float curvePt = std::powf(x, decayExp);
    decayCurve[i] = data.sustainLevel + (curvePt * decayHeight);
    x -= dX;
  }
  // 3. calculate the release LUT
  const float releaseExp = std::log(data.releaseCurve) / std::log(0.5f);
  releaseCurveLength =
      (int)(SampleRate::get() * (double)(data.releaseMs / 1000.0f));
  dX = 1.0f / (float)releaseCurveLength;
  x = 1.0f;
  for (int i = 0; i < releaseCurveLength; ++i) {
    releaseCurve[i] = std::powf(x, releaseExp) * data.sustainLevel;
    x -= dX;
  }
}
// helper for smooth voice stealing

static float _phaseMsForStealLevel(float lvl, EnvelopeLUT* env) {
  /*
    currentMs = samples * 1000 / sampleRate
    and t = currentMs / attackMs
    value = t^(log(midpoint)/log(0.5))
    t = value^(log(0.5) / log(midpoint))
    currentMs = attackMs * t
  */
  const float t =
      std::powf(lvl, std::log(0.5f) / std::log(env->getAttackCurve()));
  return env->getAttackMs() * t;
}

//===================================================

AHDSREnvelope::AHDSREnvelope(EnvelopeLUT* p, int idx) : lut(p), index(idx) {}

void AHDSREnvelope::steal() {
  gateIsOn = true;
  phaseMs = _phaseMsForStealLevel(lastOutput, lut);
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
    lastOutput = lut->getSample(currentPhase, phaseSamples);
  }
  ++phaseSamples;
}
