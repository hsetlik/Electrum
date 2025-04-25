#pragma once
#include "Electrum/GUI/GUITypedefs.h"
#include "Electrum/GUI/LookAndFeel/ElectrumLnF.h"
#include "Electrum/GUI/Modulation/ModSourceButton.h"
#include "Electrum/Identifiers.h"
#include "Electrum/Shared/ElectrumState.h"
#include "Electrum/Shared/GraphingData.h"
#include "juce_events/juce_events.h"
#include "juce_gui_basics/juce_gui_basics.h"

// the small graph of the current LFO (not for editing)
class LFOThumbnail : public Component,
                     public GraphingData::Listener,
                     public juce::AsyncUpdater {
private:
  ElectrumState* const state;
  String shapeStringID;
  float currentPhase = 0.0f;
  std::vector<fpoint_t> shapePts;
  void loadShapePoints();

public:
  const int lfoID;
  LFOThumbnail(ElectrumState* s, int idx);
  ~LFOThumbnail() override;
  void graphingDataUpdated(GraphingData* gd) override;
  void paint(juce::Graphics& g) override;
  void handleAsyncUpdate() override;
  void resized() override;
  void mouseDoubleClick(const juce::MouseEvent& me) override;
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
  frect_t thumbBounds;

public:
  const int lfoID;
  LFOComponent(ElectrumState* s, int idx);
  ~LFOComponent() override;

  void timerCallback() override;
  void resized() override;
  void paint(juce::Graphics& g) override;
};

//---------------------------------------------------------

class LFOTabs : public Component {
private:
  juce::OwnedArray<LFOComponent> lfos;
  juce::OwnedArray<ModSourceButton> buttons;
  //---------------
  int selectedLfo = -1;
  void setSelected(int idx);

public:
  LFOTabs(ElectrumState* s);
  void resized() override;
};
