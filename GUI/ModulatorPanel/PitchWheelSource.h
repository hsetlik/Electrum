#pragma once
#include "../../Parameters/ElectrumValueTree.h"
#include "../Modulation/ModulationSourceComponent.h"
#include "juce_core/juce_core.h"
class PitchWheelSource : public Component
{
private:
  EVT *const state;
  Label label;
  ModulationSourceComponent sourceComp;

public:
  PitchWheelSource(EVT *tree)
      : state(tree), label("pitch_wheel_label", "Pitch Wheel"),
        sourceComp(tree, IDs::pitchWheelSource.toString())
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
