#pragma once
#include "../AudioUtil.h"

struct ahdsr_data_t {
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
};

enum ahdsr_phase_t { Attack, Hold, Decay, Sustain, Release, Idle };

// our per-voice object
class AHDSREnvelope {
private:
  ahdsr_data_t* const params;
  float lastOutput = 0.0f;
  float currentVelocity = 0.0f;
  bool gateIsOn = false;
  size_t samplesSinceGateChange = 0;
  bool inKillQuickMode = false;
  float killQuickDelta = 0.00001f;
  ahdsr_phase_t currentPhase = ahdsr_phase_t::Idle;
  float phaseMs = 0.0f;
  float msDelta = 1000.0f / 44100.0f;
  float prevAttackCurve;
  float prevDecayCurve;
  float prevReleaseCurve;

  float attackExp;
  float decayExp;
  float releaseExp;

  float _getEnvelopeSample();

public:
  const int index;
  AHDSREnvelope(ahdsr_data_t* p, int idx);
  bool isFinished() const { return ((!gateIsOn) && fequal(lastOutput, 0.0f)); }
  void gateStart(float velocity = 1.0f) {
    currentVelocity = velocity;
    gateIsOn = true;
    samplesSinceGateChange = 0;
    phaseMs = 0.0f;
    currentPhase = ahdsr_phase_t::Attack;
  }
  void gateEnd() {
    gateIsOn = false;
    samplesSinceGateChange = 0;
  }
  // call this on sample rate changes to keep timing accurate
  void sampleRateChanged(double sr) { msDelta = (float)(1000.0 / sr); }
  void tick();
  float getCurrentSample() const { return lastOutput; }
  void steal();
  void killQuick();
};
