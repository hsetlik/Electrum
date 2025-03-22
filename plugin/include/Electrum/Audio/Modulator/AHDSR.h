#pragma once
#include "../AudioUtil.h"
#include "Electrum/Identifiers.h"
#include "juce_events/juce_events.h"

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

constexpr int maxAtkSamples = (int)(96000.0 * (double)(ATTACK_MS_MAX / 1000.0));
constexpr int maxDecaySamples =
    (int)(96000.0 * (double)(DECAY_MS_MAX / 1000.0));
constexpr int maxReleaseSamples =
    (int)(96000.0 * (double)(RELEASE_MS_MAX / 1000.0));

// change-listening class that pre-computes a LUT
// for each envelope which each voice can then
// access with just a sample index
class EnvelopeLUT : public juce::AsyncUpdater {
private:
  // the user-set parameters
  ahdsr_data_t data;
  // maximum lengths for our 3 arrays
  // arrays for our 3 curves and their lengths
  int atkCurveLength = 0;
  float atkCurve[maxAtkSamples];
  int decayCurveLength = 0;
  float decayCurve[maxDecaySamples];
  int releaseCurveLength = 0;
  float releaseCurve[maxReleaseSamples];
  int holdLengthSamples = 0;
  // math happens here
  void _computeLUTs();

public:
  EnvelopeLUT();
  void handleAsyncUpdate() override { _computeLUTs(); }
  // getters
  float getAttackMs() const { return data.attackMs; }
  float getAttackCurve() const { return data.attackCurve; }
  float getHoldMs() const { return data.holdMs; }
  float getDecayMs() const { return data.decayMs; }
  float getDecayCurve() const { return data.decayCurve; }
  float getSustainLevel() const { return data.sustainLevel; }
  float getReleaseMs() const { return data.releaseMs; }
  float getReleaseCurve() const { return data.releaseCurve; }
  float getVelTracking() const { return data.velTracking; }
  // setters
  void setAttackMs(float value);
  void setAttackCurve(float value);
  void setHoldMs(float value);
  void setDecayMs(float value);
  void setDecayCurve(float value);
  void setSustainLevel(float value);
  void setVelTracking(float value) { data.velTracking = value; }
  void setReleaseMs(float value);
  void setReleaseCurve(float value);
  // voices should use this
  float getSample(ahdsr_phase_t& currentPhase, int& phaseSamples) const;
};
//==================================================================
// our per-voice object
class AHDSREnvelope {
private:
  EnvelopeLUT* const lut;
  float lastOutput = 0.0f;
  float currentVelocity = 0.0f;
  bool gateIsOn = false;
  bool inKillQuickMode = false;
  float killQuickDelta = 0.00001f;
  ahdsr_phase_t currentPhase = ahdsr_phase_t::Idle;
  int phaseSamples = 0;
  float phaseMs = 0.0f;
  float msDelta = 1000.0f / 44100.0f;
  float prevAttackCurve;
  float prevDecayCurve;
  float prevReleaseCurve;

  float attackExp;
  float decayExp;
  float releaseExp;

  // heavy lifting happens here, basically just a finite state machine
  float _getEnvelopeSample();

public:
  const int index;
  AHDSREnvelope(EnvelopeLUT* p, int idx);
  bool isFinished() const { return ((!gateIsOn) && fequal(lastOutput, 0.0f)); }
  void gateStart(float velocity = 1.0f) {
    currentVelocity = velocity;
    gateIsOn = true;
    phaseMs = 0.0f;
    phaseSamples = 0;
    currentPhase = ahdsr_phase_t::Attack;
  }
  void gateEnd() {
    gateIsOn = false;
    currentPhase = ahdsr_phase_t::Release;
    phaseSamples = 0;
  }
  // call this on sample rate changes to keep timing accurate
  void sampleRateChanged(double sr) { msDelta = (float)(1000.0 / sr); }
  void tick();
  float getCurrentSample() const { return lastOutput; }
  void steal();
  void killQuick();
};
