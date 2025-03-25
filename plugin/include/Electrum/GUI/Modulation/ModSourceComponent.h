#pragma once
#include "Electrum/Shared/ElectrumState.h"
#include "juce_graphics/juce_graphics.h"
#include "juce_gui_basics/juce_gui_basics.h"

/* the draggable component representing a modulation source
 * */

class ModSourceComponent : public Component,
                           public juce::SettableTooltipClient {
private:
  ElectrumState* const state;

public:
  const int sourceID;
  ModSourceComponent(ElectrumState* s, int src);
  void paint(juce::Graphics& g) override;
  void mouseDown(const juce::MouseEvent& e) override;
};
