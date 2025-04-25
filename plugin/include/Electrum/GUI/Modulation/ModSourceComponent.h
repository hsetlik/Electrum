#pragma once
#include "Electrum/Identifiers.h"
#include "Electrum/Shared/ElectrumState.h"
#include "juce_graphics/juce_graphics.h"
#include "juce_gui_basics/juce_gui_basics.h"

/* the draggable component representing a modulation source
 * */

class ModSourceComponent : public Component,
                           public juce::SettableTooltipClient {
public:
  const int sourceID;
  ModSourceComponent(ElectrumState* s, int src);
  // resizing helpers
  void placeCenteredAtMaxSize(irect_t bounds);
  void paint(juce::Graphics& g) override;
  void mouseDown(const juce::MouseEvent& e) override;
};
