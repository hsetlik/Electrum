#pragma once
#include "../Modulation/DestinationSlider.h"
#include "Electrum/GUI/GUITypedefs.h"
#include "Electrum/GUI/Util/PowerButton.h"
#include "Electrum/Identifiers.h"
#include "juce_graphics/juce_graphics.h"

class FilterRoutingToggle : public juce::Button {
public:
  FilterRoutingToggle() : juce::Button("routingToggle") {
    setClickingTogglesState(true);
  }
  void paintButton(juce::Graphics& g, bool, bool) override;
};

class FilterRoutingComp : public Component {
private:
  juce::OwnedArray<FilterRoutingToggle> buttons;
  juce::OwnedArray<apvts::ButtonAttachment> attachments;
  std::vector<BoundedAttString> labels;

public:
  FilterRoutingComp(ElectrumState* s, int index);
  void resized() override;
  void paint(juce::Graphics& g) override;
};
//==================================

class FilterComp : public Component {
private:
  FilterRoutingComp router;

  DestinationSlider sCutoff;
  DestinationSlider sResonance;
  DestinationSlider sGain;
  BoundedAttString nameStr;
  PowerButton btn;
  button_attach_ptr bAttach;

  juce::ComboBox typeBox;
  combo_attach_ptr cAttach;

public:
  FilterComp(ElectrumState* s, int idx);
  void paint(juce::Graphics& g) override;
  void resized() override;
};

//=======================================

class FilterTabs : public juce::TabbedComponent {
public:
  FilterTabs(ElectrumState* s);
};
