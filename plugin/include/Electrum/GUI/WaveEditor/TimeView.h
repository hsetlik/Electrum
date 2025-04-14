#pragma once

#include "Electrum/Audio/AudioUtil.h"
#include "Electrum/GUI/WaveEditor/WaveEdiorContext.h"

#define TIME_VIEW_PTS 512

class TimeView : public WaveEditListener {
protected:
  int displayedWaveIndex = -1;
  float currentWave[TABLE_SIZE];
  juce::Path p;

public:
  TimeView(ValueTree& vt);
  ~TimeView() override {}
  void frameWasFocused(int idx) override;
  void paint(juce::Graphics& g) override;
};

//=========================================================================
