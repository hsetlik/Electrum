#pragma once
#include "../../Parameters/ElectrumValueTree.h"
#include "../Modulation/ModulationSourceComponent.h"
class ModWheelSource : public Component
{
private:
  EVT *const state;
  Label label;
  ModulationSourceComponent sourceComp;

public:
  ModWheelSource(EVT *tree)
      : state(tree), label("mod_wheel_label", "Mod Wheel"),
        sourceComp(tree, IDs::modWheelSource.toString())
  {
    juce::ignoreUnused(state);
    addAndMakeVisible(label);
    addAndMakeVisible(sourceComp);
  }

  void resized() override
  {
    auto lBounds = getLocalBounds().toFloat();
    label.setBounds(lBounds.removeFromTop(17.0f).toNearestInt());
    const int x = (int)lBounds.getX();
    const int y = (int)lBounds.getY();
    const int srcWidth = 35;
    sourceComp.setBounds(x, y, srcWidth, srcWidth);
  }
};
