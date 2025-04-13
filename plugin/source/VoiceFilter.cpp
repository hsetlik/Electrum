#include "Electrum/Audio/Filters/VoiceFilter.h"
#include "Electrum/Audio/AudioUtil.h"
#include "Electrum/Identifiers.h"
#include "Electrum/Shared/CommonAudioData.h"

void VoiceFilter::prepareCutoff() {
  const float _cutoff =
      AudioUtil::signed_flerp(FILTER_CUTOFF_MIN, FILTER_CUTOFF_MAX,
                              params->baseCutoff, modState.cutoffMod);
  if (!fequal(_cutoff, workingCutoff)) {
    workingCutoff = _cutoff;
    ladder.setCutoffHz(workingCutoff);
  }
}

void VoiceFilter::prepareResonance() {
  const float _res = AudioUtil::signed_flerp(
      FILTER_RES_MIN, FILTER_RES_MAX, params->baseResLin, modState.resMod);
  if (!fequal(_res, workingRes)) {
    workingRes = _res;
    ladder.setResonance(workingRes);
  }
}

void VoiceFilter::prepareGain() {
  static const float minGainLinear =
      juce::Decibels::decibelsToGain(FILTER_DB_MIN);
  static const float maxGainLinear =
      juce::Decibels::decibelsToGain(FILTER_DB_MAX);
  const float _gain = AudioUtil::signed_flerp(
      minGainLinear, maxGainLinear, params->baseGainLin, modState.gainMod);
  if (!fequal(_gain, workingGainLin)) {
    workingGainLin = _gain;
  }
}

float VoiceFilter::processChannel(float input, int channel) {
  float raw = ladder.processMono(input, channel);
  return raw * workingGainLin;
}

//===================================================
VoiceFilter::VoiceFilter(shared_filter_params* p) : params(p) {
  ladder.prepare(44100.0);
  prepareCutoff();
  prepareResonance();
  prepareGain();
}

void VoiceFilter::setCutoffMod(float val) {
  if (!fequal(modState.cutoffMod, val)) {
    modState.cutoffMod = val;
  }
  prepareCutoff();
}
void VoiceFilter::setResonanceMod(float val) {
  if (!fequal(modState.resMod, val)) {
    modState.resMod = val;
  }
  prepareResonance();
}
void VoiceFilter::setGainMod(float val) {
  if (!fequal(modState.gainMod, val)) {
    modState.gainMod = val;
  }
  prepareGain();
}

void VoiceFilter::processStereo(float& left, float& right) {
  if (!params->active)
    return;
  left = processChannel(left, 0);
  right = processChannel(right, 1);
}
