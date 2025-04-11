#pragma once
#include "../Util/ModalParent.h"
// an editor/viewer for one of our wavetable oscillators
class WaveEditor : public Component {
private:
  ElectrumState* const state;
  Wavetable* const wavetable;
  const int oscID;
  // this ValueTree holds the state of our wavetable editing
  ValueTree waveTree;

public:
  WaveEditor(ElectrumState* s, Wavetable* wt, int idx);
  ~WaveEditor() override;
  void resized() override;
  void paint(juce::Graphics& g) override;
};
