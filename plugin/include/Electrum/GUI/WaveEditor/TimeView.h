#pragma once

#include "Electrum/Audio/AudioUtil.h"
#include "WaveThumbnail.h"

#define TIME_VIEW_PTS 512

class TimeView : public Component, public WaveThumbnailBar::Listener {
private:
  ValueTree& waveTree;
  int displayedWaveIndex = -1;
  float currentWave[TABLE_SIZE];
  juce::Path p;

public:
  TimeView(ValueTree& vt);
  void frameWasFocused(int idx) override;
  void paint(juce::Graphics& g) override;
};

