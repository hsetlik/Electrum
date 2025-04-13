#pragma once
#include "../Modulation/DestinationSlider.h"
#include "Electrum/GUI/GUITypedefs.h"
#include "Electrum/GUI/Util/PowerButton.h"
#include "Electrum/Identifiers.h"

class FilterComp : public Component {
  DestinationSlider sCutoff;
  DestinationSlider sResonance;
  DestinationSlider sGain;
  BoundedAttString nameStr;
  PowerButton btn;
  button_attach_ptr bAttach;

public:
  FilterComp(ElectrumState* s, int idx);
  void paint(juce::Graphics& g) override;
  void resized() override;
};
