#pragma once
#include "Electrum/GUI/GUITypedefs.h"
#include "Electrum/GUI/LookAndFeel/ElectrumLnF.h"
#include "Electrum/Identifiers.h"
#include "Electrum/Shared/ElectrumState.h"
#include "Electrum/Shared/GraphingData.h"
#include "juce_events/juce_events.h"

// the small graph of the current LFO (not for editing)
class LFOThumbnail : public Component, public GraphingData::Listener {
private:
  ElectrumState* const state;
  String shapeStringID;
  float currentPhase = 0.0f;
  std::vector<fpoint_t> shapePts;
  void loadShapePoints();

public:
  const int lfoID;
  LFOThumbnail(ElectrumState* s, int idx);
  void graphingDataUpdated(GraphingData* gd) override;
  void paint(juce::Graphics& g) override;
};

//---------------------------------------------------------

#define LFO_STRING_CHECK_HZ 6
class LFOComponent : public Component, public juce::Timer {
private:
  ElectrumState* const state;

  BoundedAttString nameLabel;

  BoundedAttString freqLabel;
  juce::Slider freqSlider;
  slider_attach_ptr freqAttach;

  BoundedAttString trigModeLabel;
  juce::ComboBox trigModeCB;
  combo_attach_ptr trigModeAttach;

  std::unique_ptr<LFOThumbnail> thumb;

  size_t lastLfoHash = 0;
  ElectrumLnF lnf;

public:
  const int lfoID;
  LFOComponent(ElectrumState* s, int idx);
  ~LFOComponent() override;

  void timerCallback() override;
  void resized() override;
  void paint(juce::Graphics& g) override;
};

//---------------------------------------------------------

class LFOTabs : public juce::TabbedComponent {
public:
  LFOTabs(ElectrumState* s);
};
