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
  // Slider stuff-------------------------------------------------
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
  // ComboBox stuff------------------------------------------
  void drawComboBox(juce::Graphics& g,
                    int width,
                    int height,
                    bool isButtonDown,
                    int buttonX,
                    int buttonY,
                    int buttonW,
                    int buttonH,
                    juce::ComboBox& cb) override;
  juce::Font getComboBoxFont(juce::ComboBox& cb) override;
  // Scroll Bar------------------------------------
  void drawScrollbar(juce::Graphics& g,
                     juce::ScrollBar& scrollbar,
                     int x,
                     int y,
                     int width,
                     int height,
                     bool isScrollbarVertical,
                     int thumbStartPosition,
                     int thumbSize,
                     bool isMouseOver,
                     bool isMouseDown) override;
};
