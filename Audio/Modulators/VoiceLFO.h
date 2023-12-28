#pragma once
#include "../../Parameters/AudioSystem.h"
#include "../../Parameters/ElectrumValueTree.h"

class VoiceLFO
{
private:
  EVT *const state;
  const float sampleRate;
  float lastOutput;
  float currentPhase;
  bool inQuickKill;
  float quickKillDelta;
  void startQuickKill();
  float getNextSample();

public:
  const int index;
  VoiceLFO(EVT *tree, int idx);
  ~VoiceLFO() {}
  void tick();
  float getCurrentValue() { return lastOutput; }
  void retrigger() { startQuickKill(); }
};
