#pragma once

#include "Electrum/Audio/AudioUtil.h"
#include "Electrum/GUI/WaveEditor/PointEditor.h"
#include "Electrum/GUI/WaveEditor/WaveEdiorContext.h"
#include "Electrum/Shared/PointwiseWave.h"

#define TIME_VIEW_PTS 512

class TimeView : public WaveEditListener {
protected:
  int displayedWaveIndex = -1;
  std::unique_ptr<PointEditor> editor;

public:
  TimeView(ValueTree& vt);
  ~TimeView() override {}
  void frameWasFocused(int idx) override;
  void resized() override;
};

//=========================================================================
