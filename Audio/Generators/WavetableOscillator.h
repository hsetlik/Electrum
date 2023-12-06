#pragma once
#include "../../Parameters/ElectrumValueTree.h"

class WavetableOscillator {
private:
  EVT *const state;
  const int index;
  float phase;
  float baseWavetablePos;
  float baseLevel;

public:
  // these make things slightly easier ill explain later
  WavetableOscillator(EVT *t, int idx);
  float getNextSample(double freq, double sampleRate, float levelMod,
                      float posMod);
  void setBasePosition(float value) { baseWavetablePos = value; }
  void setBaseLevel(float value) { baseLevel = value; }
  void updateBasePos();
  void updateBaseLevel();
  String getPosParamName() const {
    return IDs::oscillatorPos.toString() + String(index);
  }
  String getLevelParamName() const {
    return IDs::oscillatorLevel.toString() + String(index);
  }
};
