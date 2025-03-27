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
  void drawRotarySlider(juce::Graphics& g,
                        int x,
                        int y,
                        int width,
                        int height,
                        float sliderPosProportional,
                        float rotaryStartAngle,
                        float rotaryEndAngle,
                        juce::Slider& s) override;
  juce::Slider::SliderLayout getSliderLayout(juce::Slider& slider) override;
  //--------------------------------------------------------------
};
