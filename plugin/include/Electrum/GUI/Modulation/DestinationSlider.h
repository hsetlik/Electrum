#pragma once
#include "DepthSliderStack.h"
#include "Electrum/GUI/Modulation/ModContextComponent.h"
#include "Electrum/Identifiers.h"
#include "Electrum/Shared/ElectrumState.h"
#include "juce_audio_processors/juce_audio_processors.h"
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

class DestinationSlider : public ModDestAttachment,
                          public juce::DragAndDropTarget {
private:
  ElectrumState* const state;
  juce::Slider slider;
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
