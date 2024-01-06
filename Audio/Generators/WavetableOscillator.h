#pragma once
#include "../../Parameters/ElectrumValueTree.h"

class WavetableOscillator
{
private:
  EVT *const state;
  const int index;
  float phase;
  float baseWavetablePos;
  float baseLevel;
  float basePan;
  float baseCoarseTune;
  float baseFineTune;
  float lastPositionFinal;

public:
  WavetableOscillator(EVT *t, int idx);
  float getNextSample(double freq, double sampleRate, float levelMod, float posMod);
  float getNextSample(int midiNote, double sampleRate, float levelMod, float posMod,
                      float coarseMod, float fineMod);
  // equivalent to above but deals with pan as well
  void renderSampleStereo(int midiNote, double sampleRate, float levelMod, float posMod,
                          float panMod, float coarseMod, float fineMod, float &left, float &right);
  void setBasePosition(float value) { baseWavetablePos = value; }
  void setBaseLevel(float value) { baseLevel = value; }
  void updateBasePos();
  void updateBaseLevel();
  void updateBasePan();
  void updateBaseCoarse();
  void updateBaseFine();
  String getPosParamName() const { return IDs::oscillatorPos.toString() + String(index); }
  String getLevelParamName() const { return IDs::oscillatorLevel.toString() + String(index); }
  float getLastPosition() { return lastPositionFinal; }
  float getBasePosition() { return baseWavetablePos; }
};
