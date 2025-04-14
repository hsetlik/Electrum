#pragma once

#include "Electrum/Audio/Wavetable.h"
#include "Electrum/GUI/WaveEditor/WaveEdiorContext.h"
class FFTView : public WaveEditListener {
  freq_bin_t waveBins[AUDIBLE_BINS];
  int numMagPoints = 256;

public:
  FFTView(ValueTree& vt);
  void frameWasFocused(int idx) override;
  void paint(juce::Graphics& g) override;
};
