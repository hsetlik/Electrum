#include "Electrum/GUI/Modulation/DestinationSlider.h"
#include "juce_graphics/juce_graphics.h"
#include "juce_gui_basics/juce_gui_basics.h"

// Depth Sliders ====================================
static void _strokeArc(juce::Graphics& g,
                       juce::Rectangle<float>& area,
                       float startAngle,
                       float endAngle,
                       float strokeWidth) {
  juce::Path p;
  p.addArc(area.getX(), area.getY(), area.getWidth(), area.getHeight(),
           startAngle, endAngle, true);
  juce::PathStrokeType pst(strokeWidth, juce::PathStrokeType::mitered,
                           juce::PathStrokeType::rounded);
  g.strokePath(p, pst);
}

void DepthSliderLookAndFeel::drawRotarySlider(juce::Graphics& g,
                                              int x,
                                              int y,
                                              int width,
                                              int height,
                                              float sliderPosProportional,
                                              float rotaryStartAngle,
                                              float rotaryEndAngle,
                                              juce::Slider&) {
  //  step 1: Draw the main arc
  juce::Rectangle<int> iArea(x, y, width, height);
  auto fArea = iArea.toFloat();
  g.setColour(juce::Colours::aliceblue);
  auto sliderArea = fArea.reduced(5.0f);
  _strokeArc(g, sliderArea, rotaryStartAngle, rotaryEndAngle, 2.0f);
  auto toAngle = flerp(rotaryStartAngle, rotaryEndAngle, sliderPosProportional);
  // step 2: Draw the thumb
  auto radius = (fArea.getWidth() / 2.0f) * 0.95f;
  juce::Point<float> thumbPoint(
      fArea.getCentreX() +
          radius * std::cos(toAngle - juce::MathConstants<float>::halfPi),
      fArea.getCentreY() +
          radius * std::sin(toAngle - juce::MathConstants<float>::halfPi));
  const float thumbWidth = 9.0f;
  g.setColour(juce::Colours::darkseagreen);
  g.fillEllipse(
      juce::Rectangle<float>(thumbWidth, thumbWidth).withCentre(thumbPoint));
}

static juce::Slider::RotaryParameters _getDepthSliderParams() {
  juce::Slider::RotaryParameters params;
  params.startAngleRadians = juce::MathConstants<float>::pi * 1.35f;
  params.endAngleRadians = juce::MathConstants<float>::pi * 2.65f;
  params.stopAtEnd = true;
  return params;
}

DepthSlider::DepthSlider(int src)
    : juce::Slider(juce::Slider::Rotary, juce::Slider::NoTextBox),
      sourceID(src) {
  setLookAndFeel(&lnf);
  setRange(-1.0f, 1.0f, 0.0001f);
  setValue(0.0f);
  setRotaryParameters(_getDepthSliderParams());
}

DepthSlider::~DepthSlider() {
  setLookAndFeel(nullptr);
}
//===================================================
