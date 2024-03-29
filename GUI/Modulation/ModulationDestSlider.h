#pragma once
#include "../../Parameters/ElectrumValueTree.h"
#include "DepthSliderStack.h"

class ModulationDestSlider : public Component, public DragAndDropTarget
{
private:
  EVT *const state;
  const String paramID;
  DepthSliderStack stack;

  bool hasModulationFrom(const String &srcID);
  void addModulationFor(const String &srcID);
  void removeModulationFrom(const String &srcID);

public:
  ModulationDestSlider(EVT *tree, const String &id);
  ~ModulationDestSlider() override;
  void resized() override;
  void paint(Graphics &g) override;
  void itemDropped(const juce::DragAndDropTarget::SourceDetails &dragSourceDetails) override;
  void itemDragEnter(const juce::DragAndDropTarget::SourceDetails &) override {}
  void itemDragExit(const juce::DragAndDropTarget::SourceDetails &) override {}
  void itemDragMove(const juce::DragAndDropTarget::SourceDetails &) override {}
  bool
  isInterestedInDragSource(const juce::DragAndDropTarget::SourceDetails &dragSourceDetails) override
  {
    String srcName = dragSourceDetails.description;
    return !hasModulationFrom(srcName);
  }
  bool shouldDrawDragImageWhenOver() override { return true; }
  Slider paramSlider;

private:
  sAttachPtr attach;
};
