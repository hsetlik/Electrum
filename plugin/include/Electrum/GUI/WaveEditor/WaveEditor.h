#pragma once
#include "../Util/ModalParent.h"
// an editor/viewer for one of our wavetable oscillators
class WaveEditor : public Component {
private:
  ElectrumState* const state;
  Wavetable* const wavetable;
  const int oscID;

public:
  WaveEditor(ElectrumState* s, Wavetable* wt, int idx);
  void resized() override;
};

