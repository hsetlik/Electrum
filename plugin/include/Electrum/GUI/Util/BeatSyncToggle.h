#pragma once

#include "juce_graphics/juce_graphics.h"
#include "juce_gui_basics/juce_gui_basics.h"

class BeatSyncToggle : public juce::Button {
public:
  BeatSyncToggle() : juce::Button("BeatSyncBtn") {}
  void paintButton(juce::Graphics& g, bool, bool) override;
};
