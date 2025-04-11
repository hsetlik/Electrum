#pragma once

#include "../Modulation/DestinationSlider.h"
#include "Electrum/GUI/LookAndFeel/BinaryGraphics.h"
#include "juce_core/system/juce_PlatformDefs.h"
#include "WavetableGraph.h"

class OscillatorPanel : public Component, public juce::ComboBox::Listener {
private:
  ElectrumState* const state;

  // sliders
  DestinationSlider sCoarse;
  DestinationSlider sFine;
  DestinationSlider sPos;
  DestinationSlider sLevel;
  DestinationSlider sPan;

  // graph
  WavetableGraph graph;

  // edit/display stuff
  juce::ComboBox wavetableCB;
  ImgButton editBtn;

public:
  const int oscID;
  OscillatorPanel(ElectrumState* s, int id);
  void resized() override;
  void paint(juce::Graphics& g) override;
  void comboBoxChanged(juce::ComboBox* cb) override;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscillatorPanel)
};
