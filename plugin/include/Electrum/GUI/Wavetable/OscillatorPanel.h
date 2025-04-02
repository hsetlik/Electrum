#pragma once

#include "../Modulation/DestinationSlider.h"
#include "juce_core/system/juce_PlatformDefs.h"
// #include "WavetableGraph.h"

class OscillatorPanel : public Component {
private:
  ElectrumState* const state;

  DestinationSlider sCoarse;
  DestinationSlider sFine;
  DestinationSlider sPos;
  DestinationSlider sLevel;
  DestinationSlider sPan;

  // WavetableGraph graph;

public:
  const int oscID;
  OscillatorPanel(ElectrumState* s, int id);
  void resized() override;
  void paint(juce::Graphics& g) override;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscillatorPanel)
};
