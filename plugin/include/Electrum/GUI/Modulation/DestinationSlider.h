#pragma once
#include "DepthSliderStack.h"
#include "Electrum/GUI/Modulation/ModContextComponent.h"
#include "Electrum/Identifiers.h"
#include "Electrum/Shared/ElectrumState.h"
#include "juce_gui_basics/juce_gui_basics.h"

class DestinationSlider : public ModDestAttachment,
                          public juce::DragAndDropTarget {
private:
  ElectrumState* const state;
  juce::Slider slider;
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
    // DLog::log("Asked about drag source with ID: " + String(srcID));
    return !depthSliders.hasComponentsForSrc(srcID);
  }
};
