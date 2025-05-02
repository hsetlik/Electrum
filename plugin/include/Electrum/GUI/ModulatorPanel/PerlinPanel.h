#pragma once

#include "Electrum/GUI/GUITypedefs.h"
#include "Electrum/Identifiers.h"
#include "../../Shared/GraphingData.h"
#include "Electrum/Shared/ElectrumState.h"
#include "juce_events/juce_events.h"

#define PERLIN_GRAPH_POINTS 128
class PerlinGraph : public Component,
                    public GraphingData::Listener,
                    public juce::AsyncUpdater {
private:
  ElectrumState* const state;
  size_t head = 0;
  std::array<float, PERLIN_GRAPH_POINTS> levels = {};

public:
  const int perlinID;
  PerlinGraph(ElectrumState* s, int idx);
  ~PerlinGraph() override;
  void graphingDataUpdated(GraphingData* gd) override;
  void paint(juce::Graphics& g) override;
  void handleAsyncUpdate() override;
};

//===================================================

class PerlinComponent : public Component {
private:
  PerlinGraph graph;
  BoundedAttString freqLabel;
  juce::Slider freqSlider;
  slider_attach_ptr freqAttach;
  BoundedAttString octLabel;
  juce::Slider octSlider;
  slider_attach_ptr octAttach;
  BoundedAttString lacLabel;
  juce::Slider lacSlider;
  slider_attach_ptr lacAttach;

public:
  PerlinComponent(ElectrumState* s, int idx);
  void resized() override;
  void paint(juce::Graphics& g) override;
};
