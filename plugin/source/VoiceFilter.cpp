#include "Electrum/Audio/Filters/VoiceFilter.h"
#include "Electrum/Audio/AudioUtil.h"
#include "Electrum/Identifiers.h"
#include "Electrum/Shared/CommonAudioData.h"

void VoiceFilter::prepareCutoff() {
  const float _cutoff = AudioUtil::signed_flerp(
      FILTER_CUTOFF_MIN, FILTER_CUTOFF_MAX, baseCutoff, modState.cutoffMod);
  if (!fequal(_cutoff, workingCutoff)) {
    workingCutoff = _cutoff;
    switch (currentFilterType) {
      case LadderLPLinear:
        ladderBasic.setCutoffHz(workingCutoff);
        break;
      case LadderLPSaturated:
        ladderLoPass.setCutoffHz(workingCutoff);
        break;
      case LadderHP:
        ladderHP.setCutoffHz(workingCutoff);
        break;
      case LadderBP:
        ladderHP.setCutoffHz(workingCutoff);
        break;
      default:
        return;
    }
  }
}

void VoiceFilter::prepareResonance() {
  const float _res = AudioUtil::signed_flerp(FILTER_RES_MIN, FILTER_RES_MAX,
                                             baseRes, modState.resMod);
  if (!fequal(_res, workingRes)) {
    workingRes = _res;
    switch (currentFilterType) {
      case LadderLPLinear:
        ladderBasic.setResonance(workingRes);
        break;
      case LadderLPSaturated:
        ladderLoPass.setResonance(workingRes);
        break;
      case LadderHP:
        ladderHP.setResonance(workingRes);
        break;
      case LadderBP:
        ladderHP.setResonance(workingRes);
        break;
      default:
        return;
    }
  }
}

void VoiceFilter::prepareGain() {
  static const float minGainLinear =
      juce::Decibels::decibelsToGain(FILTER_DB_MIN);
  static const float maxGainLinear =
      juce::Decibels::decibelsToGain(FILTER_DB_MAX);
  const float _gain = AudioUtil::signed_flerp(minGainLinear, maxGainLinear,
                                              baseGain, modState.gainMod);
  if (!fequal(_gain, workingGainLin)) {
    workingGainLin = _gain;
  }
}

float VoiceFilter::processChannel(float input, int channel) {
  float raw;
  switch (currentFilterType) {
    case LadderLPLinear:
      raw = ladderBasic.processMono(input, channel);
      break;
    case LadderLPSaturated:
      raw = ladderLoPass.processMono(input, channel);
      break;
    case LadderHP:
      raw = ladderHP.processMono(input, channel);
      break;
    case LadderBP:
      raw = ladderHP.processMono(input, channel);
    default:
      break;
  }
  return raw * workingGainLin;
}

//===================================================
VoiceFilter::VoiceFilter(shared_filter_params* p) : params(p) {
  ladderBasic.prepare(44100.0);
  ladderLoPass.prepare(44100.0);
  ladderHP.prepare(44100.0);
  prepareCutoff();
  prepareResonance();
  prepareGain();
}

void VoiceFilter::updateForBlock() {
  if (!fequal(params->baseCutoff, baseCutoff)) {
    baseCutoff = params->baseCutoff;
    prepareCutoff();
  }
  if (!fequal(params->baseResLin, baseRes)) {
    baseRes = params->baseResLin;
    prepareResonance();
  }
  if (!fequal(params->baseGainLin, baseGain)) {
    baseGain = params->baseResLin;
    prepareGain();
  }
  if (currentFilterType != params->filterType) {
    currentFilterType = params->filterType;
    prepareCutoff();
    prepareResonance();
    prepareGain();
  }
}

void VoiceFilter::prepare(double sampleRate) {
  ladderBasic.prepare(sampleRate);
  ladderLoPass.prepare(sampleRate);
}

void VoiceFilter::setCutoffMod(float val) {
  if (!fequal(modState.cutoffMod, val)) {
    modState.cutoffMod = val;
    prepareCutoff();
  }
}
void VoiceFilter::setResonanceMod(float val) {
  if (!fequal(modState.resMod, val)) {
    modState.resMod = val;
    prepareResonance();
  }
}
void VoiceFilter::setGainMod(float val) {
  if (!fequal(modState.gainMod, val)) {
    modState.gainMod = val;
    prepareGain();
  }
}

void VoiceFilter::processStereo(float& left, float& right) {
  if (!params->active)
    return;
  left = processChannel(left, 0);
  right = processChannel(right, 1);
}
