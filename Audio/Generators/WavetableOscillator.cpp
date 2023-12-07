#include "WavetableOscillator.h"

WavetableOscillator::WavetableOscillator(EVT *t, int idx)
    : state(t), index(idx), baseWavetablePos(0.0f), baseLevel(1.0f) {}

float WavetableOscillator::getNextSample(double freq, double sampleRate,
                                         float levelMod, float posMod) {
  if (baseLevel + levelMod <= 0.0f)
    return 0.0f;
  phase = std::fmod(phase + (float)(freq / sampleRate), 1.0f);
  return state->getOscillatorValue(
             index, phase,
             Math::bipolarFlerp(0.0f, 1.0f, baseWavetablePos, posMod), freq,
             sampleRate) *
         Math::bipolarFlerp(0.0f, 1.0f, baseLevel, levelMod);
}

float WavetableOscillator::getNextSample(int midiNote, double sampleRate,
                                         float levelMod, float posMod,
                                         float coarseMod, float fineMod) {
  if (baseLevel + levelMod <= 0.0f)
    return 0.0f;
  float coarse = Math::bipolarFlerp(COARSE_TUNE_MIN, COARSE_TUNE_MAX,
                                    baseCoarseTune, coarseMod);
  float fine =
      Math::bipolarFlerp(FINE_TUNE_MIN, FINE_TUNE_MAX, baseFineTune, fineMod);
  auto freq = Math::midiToHz(midiNote, coarse, fine);
  phase = std::fmod(phase + (float)(freq / sampleRate), 1.0f);
  return state->getOscillatorValue(
             index, phase,
             Math::bipolarFlerp(0.0f, 1.0f, baseWavetablePos, posMod), freq,
             sampleRate) *
         Math::bipolarFlerp(0.0f, 1.0f, baseLevel, levelMod);
}

void WavetableOscillator::updateBasePos() {
  String id = IDs::oscillatorPos.toString() + String(index);
  baseWavetablePos = *state->getAPVTS()->getRawParameterValue(id);
}

void WavetableOscillator::updateBaseLevel() {
  String id = IDs::oscillatorLevel.toString() + String(index);
  baseLevel = *state->getAPVTS()->getRawParameterValue(id);
}

void WavetableOscillator::updateBaseCoarse() {
  String id = IDs::oscillatorCoarseTune.toString() + String(index);
  baseCoarseTune = *state->getAPVTS()->getRawParameterValue(id);
}

void WavetableOscillator::updateBaseFine() {
  String id = IDs::oscillatorFineTune.toString() + String(index);
  baseFineTune = *state->getAPVTS()->getRawParameterValue(id);
}
