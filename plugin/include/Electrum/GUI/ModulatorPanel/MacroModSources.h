#pragma once
#include "../Modulation/ModSourceComponent.h"
#include "Electrum/Identifiers.h"
#include "Electrum/Shared/ElectrumState.h"
#include "juce_graphics/juce_graphics.h"

/*This component holds all
 * our modulation
 * sources that don't have other
 * GUI controls. i.e. mod/pitch wheel
 * velocity, so on*/
class MacroModSources : public Component {
private:
  juce::TextLayout modWheelLabel;
  juce::TextLayout velocityLabel;
  ModSourceComponent modWheelSrc;
  ModSourceComponent velocitySrc;

public:
  MacroModSources(ElectrumState* s);
  void paint(juce::Graphics& g) override;
  void resized() override;
};
