#pragma once
#include "Electrum/Identifiers.h"
#include "Electrum/Shared/CommonAudioData.h"
#include "Ladder.h"

class VoiceFilter {
private:
  shared_filter_params* params;
  LadderLPBasic ladderBasic;
  LadderLP ladderLoPass;
  LadderHighPass ladderHP;

  // holds the current modulation state for this voice's filter
  // same idea as 'osc_mod_t' in Voice.h
  struct filt_mod_t {
    float cutoffMod = 0.0f;
    float resMod = 0.0f;
    float gainMod = 0.0f;
  };
  filt_mod_t modState;
  // for checking when we need to re-calcuatte coefficients
  // and gains
  float workingCutoff = -50000.0f;
  float workingRes = 500000.0f;
  float workingGainLin = 0.0f;
  // the base parameters that get updated once per block
  float baseCutoff;
  float baseRes;
  float baseGain;
  FilterTypeE currentFilterType = FilterTypeE::LadderLPLinear;
  // runs whatever relevant code prepares coefficients
  // for the current filter parameters
  void prepareCutoff();
  void prepareResonance();
  void prepareGain();

  float processChannel(float input, int channel);

  void reinitForType();

public:
  VoiceFilter(shared_filter_params* p);
  // call this on sample rate changes
  void prepare(double sampleRate);
  // these should be passed per-voice modulation data
  void setCutoffMod(float val);
  void setResonanceMod(float val);
  void setGainMod(float val);
  void updateForBlock();
  // the main processing callback
  void processStereo(float& left, float& right);
};
