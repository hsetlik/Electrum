#pragma once
#include "DepthSliderStack.h"
#include "Electrum/GUI/Modulation/ModContextComponent.h"
#include "Electrum/Identifiers.h"
#include "Electrum/Shared/ElectrumState.h"
#include "Electrum/Shared/GraphingData.h"
#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_events/juce_events.h"
#include "juce_gui_basics/juce_gui_basics.h"

class DestinationLabel : public Component, public juce::Label::Listener {
private:
  ElectrumState* const state;
  juce::Label label;
  std::unique_ptr<juce::ParameterAttachment> pAttach;
  // callback for the parameter attachment
  void setLabelForValue(float val);

  float currentParamVal = 0.0f;

public:
  const int destID;
  DestinationLabel(ElectrumState* s, int id);
  void labelTextChanged(juce::Label* l) override;
  void resized() override { label.setBounds(getLocalBounds()); }
};

//============================================================
class DestSliderLnF : public juce::LookAndFeel_V4 {
public:
  DestSliderLnF() = default;
  void drawRotarySlider(juce::Graphics& g,
                        int x,
                        int y,
                        int width,
                        int height,
                        float sliderPosProportional,
                        float rotaryStartAngle,
                        float rotaryEndAngle,
                        juce::Slider& slider) override;
};

class DestSliderCore : public juce::Slider,
                       public GraphingData::Listener,
                       public juce::AsyncUpdater {
private:
  float lastModNormalized = 0.0f;
  ElectrumState* state;
  const int destID;
  DestSliderLnF lnf;

public:
  DestSliderCore(ElectrumState* s, int id);
  ~DestSliderCore() override;
  void graphingDataUpdated(GraphingData* gd) override;
  void handleAsyncUpdate() override { repaint(); }
  float getNormalizedMod() const { return lastModNormalized; }
};

class DestinationSlider : public ModDestAttachment,
                          public juce::DragAndDropTarget {
private:
  ElectrumState* const state;
  DestSliderCore slider;
  DestinationLabel label;
  slider_attach_ptr attach;

public:
  DepthSliderStack depthSliders;
  DestinationSlider(ElectrumState* s, int dest);
  void reinit() override;
  void resized() override;
  void paint(juce::Graphics& g) override;
  void itemDropped(
      const juce::DragAndDropTarget::SourceDetails& dragSourceDetails) override;
  void itemDragEnter(const juce::DragAndDropTarget::SourceDetails&) override {}
  void itemDragExit(const juce::DragAndDropTarget::SourceDetails&) override {}
  void itemDragMove(const juce::DragAndDropTarget::SourceDetails&) override {}

  bool isInterestedInDragSource(const juce::DragAndDropTarget::SourceDetails&
                                    dragSourceDetails) override {
    int srcID = dragSourceDetails.description;
    return !depthSliders.hasComponentsForSrc(srcID);
  }
};
