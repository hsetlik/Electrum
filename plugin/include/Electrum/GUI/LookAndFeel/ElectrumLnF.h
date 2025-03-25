#pragma once
#include "Color.h"
#include "juce_gui_basics/juce_gui_basics.h"

// too many colons man
typedef juce::LookAndFeel_V4::ColourScheme LnFColorScheme;
typedef juce::LookAndFeel_V4::ColourScheme::UIColour UIColorE;

class ElectrumLnF : public juce::LookAndFeel_V4 {
public:
  ElectrumLnF();
  // overrides for our various drawing methods--------------------
  //--------------------------------------------------------------
};
