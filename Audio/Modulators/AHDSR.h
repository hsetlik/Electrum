#pragma once
#include "../../Parameters/AudioSystem.h"
#include "../../Parameters/DLog.h"
#include "../../Parameters/MathUtil.h"

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
  float attackCurve = ENV_CURVE_DEFAULT;
  // hold
  float holdMs = 0.0f;
  // decay
  float decayMs = DECAY_MS_DEFAULT;
  float decayCurve = ENV_CURVE_DEFAULT;

  // sustain
  float sustainLevel = SUSTAIN_LEVEL_DEFAULT;
  // velocity tracking
  float velTracking = VEL_TRACKING_DEFAULT;

  // release
  float releaseMs = RELEASE_MS_DEFAULT;
  float releaseCurve = ENV_CURVE_DEFAULT;

  // static methods to hande the logic of turning these parameters into an
  // envelope output
  static AHDSRPhase getCurrentPhase(AHDSRData *env, bool gateOn, size_t samplesSinceGateChange);
  // similar to above but takes argume in ms rather than samples
  static AHDSRPhase getPhaseForMs(AHDSRData *env, bool gateOn, float msSinceGateChange);
  // this is helpful for the voice stealing logic
  static size_t getSamplesForAttackValue(AHDSRData *env, float value);
};
