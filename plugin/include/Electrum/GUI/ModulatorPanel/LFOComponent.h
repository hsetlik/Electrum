#pragma once
#include "Electrum/GUI/GUITypedefs.h"
#include "Electrum/Shared/ElectrumState.h"
#include "Electrum/Shared/GraphingData.h"
#include "juce_events/juce_events.h"

// the small graph of the current LFO (not for editing)
class LFOThumbnail : public Component, public GraphingData::Listener {
private:
  ElectrumState* const state;
  String shapeStringID;
  float currentPhase = 0.0f;
  size_t lastLfoHash = 0;
  std::vector<fpoint_t> shapePts;
  void loadShapePoints();

public:
  const int lfoID;
  LFOThumbnail(ElectrumState* s, int idx);
  void graphingDataUpdated(GraphingData* gd) override;
  void paint(juce::Graphics& g) override;
};
