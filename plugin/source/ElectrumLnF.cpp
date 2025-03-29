#include "Electrum/GUI/LookAndFeel/ElectrumLnF.h"
#include "Electrum/Common.h"
#include "Electrum/GUI/LookAndFeel/Color.h"
#include "Electrum/Identifiers.h"
#include "juce_gui_basics/juce_gui_basics.h"

static void _logColor(const String& name, color_t color) {
  DLog::log("Color for \"" + name + "\" is:");
  DLog::log("R, G, B: " + String(color.getRed()) + ", " +
            String(color.getGreen()) + ", " + String(color.getBlue()));
}

static void _logColorScheme(const LnFColorScheme& scheme) {
  // the 9 colors in the juce ColourScheme enum
  _logColor("window bkgnd", scheme.getUIColour(UIColorE::windowBackground));
  _logColor("widget bkgnd", scheme.getUIColour(UIColorE::widgetBackground));
  _logColor("menu bkgnd", scheme.getUIColour(UIColorE::menuBackground));
  _logColor("outline", scheme.getUIColour(UIColorE::outline));
  _logColor("default text", scheme.getUIColour(UIColorE::defaultText));
  _logColor("default fill", scheme.getUIColour(UIColorE::defaultFill));
  _logColor("highlighted text", scheme.getUIColour(UIColorE::highlightedText));
  _logColor("highlighted fill", scheme.getUIColour(UIColorE::highlightedFill));
  _logColor("menu text", scheme.getUIColour(UIColorE::menuText));
}

static LnFColorScheme _getDefaultDarkScheme() {
  LnFColorScheme scheme = juce::LookAndFeel_V4::getDarkColourScheme();
  //_logColorScheme(scheme);
  return scheme;
}

static LnFColorScheme _getNightflyScheme() {
  // DLog::log("DEFAULT=================================");
  LnFColorScheme scheme = _getDefaultDarkScheme();
  scheme.setUIColour(UIColorE::windowBackground, Color::nearBlack);
  scheme.setUIColour(UIColorE::menuBackground, Color::darkBlue);
  scheme.setUIColour(UIColorE::widgetBackground, Color::nearBlack);
  scheme.setUIColour(UIColorE::outline, Color::commentGray);
  scheme.setUIColour(UIColorE::defaultText, Color::literalOrangePale);
  scheme.setUIColour(UIColorE::menuText, Color::literalOrangePale);
  scheme.setUIColour(UIColorE::defaultFill, Color::mintGreenPale);
  scheme.setUIColour(UIColorE::highlightedText, Color::assignmentPink);
  scheme.setUIColour(UIColorE::highlightedFill, Color::mintGreenBright);

  // DLog::log("NIGHTFLY=================================");
  //_logColorScheme(scheme);
  return scheme;
}

//===================================================

ElectrumLnF::ElectrumLnF() : juce::LookAndFeel_V4(_getNightflyScheme()) {}

void ElectrumLnF::drawRotarySlider(juce::Graphics& g,
                                   int x,
                                   int y,
                                   int width,
                                   int height,
                                   float sliderPosProportional,
                                   float rotaryStartAngle,
                                   float rotaryEndAngle,
                                   juce::Slider& slider) {
  const color_t trackLeft =
      slider.findColour(juce::Slider::rotarySliderOutlineColourId);
  const color_t trackRight =
      slider.findColour(juce::Slider::rotarySliderFillColourId);
  auto fBounds =
      juce::Rectangle<int>(x, y, width, height).toFloat().reduced(10);
  const float thumbAngle =
      flerp(rotaryStartAngle, rotaryEndAngle, sliderPosProportional);
  const float diameter = std::min(fBounds.getHeight(), fBounds.getWidth());
  const float outerRadius = diameter / 2.0f;
  const float innerRadius = outerRadius * 0.85f;
  // 1. draw the path for the  left half of the
  // slider track
  juce::Path tLeft;
  tLeft.addCentredArc(fBounds.getCentreX(), fBounds.getCentreY(), outerRadius,
                      outerRadius, 0.0f, thumbAngle, rotaryStartAngle, true);
  tLeft.addCentredArc(fBounds.getCentreX(), fBounds.getCentreY(), innerRadius,
                      innerRadius, 0.0f, rotaryStartAngle, thumbAngle, false);

  tLeft.closeSubPath();
  g.setColour(trackLeft);
  g.fillPath(tLeft);
  // 2. same idea for the right half
  juce::Path tRight;
  tRight.addCentredArc(fBounds.getCentreX(), fBounds.getCentreY(), outerRadius,
                       outerRadius, 0.0f, thumbAngle, rotaryEndAngle, true);
  tRight.addCentredArc(fBounds.getCentreX(), fBounds.getCentreY(), innerRadius,
                       innerRadius, 0.0f, rotaryEndAngle, thumbAngle, false);
  tRight.closeSubPath();
  g.setColour(trackRight);
  g.fillPath(tRight);
}

juce::Slider::SliderLayout ElectrumLnF::getSliderLayout(juce::Slider& slider) {
  juce::Slider::SliderLayout layout;
  int minXSpace = 0;
  int minYSpace = 0;
  auto textBoxPos = slider.getTextBoxPosition();
  if (textBoxPos == juce::Slider::TextBoxLeft ||
      textBoxPos == juce::Slider::TextBoxRight)
    minXSpace = 30;
  else
    minYSpace = 12;

  auto localBounds = slider.getLocalBounds();
  auto textBoxWidth = std::max(0, std::min(slider.getTextBoxWidth(),
                                           localBounds.getWidth() - minXSpace));
  auto textBoxHeight = std::max(
      0,
      std::min(slider.getTextBoxHeight(), localBounds.getHeight() - minYSpace));

  // set up the text box
  if (textBoxPos != juce::Slider::NoTextBox) {
    if (slider.isBar()) {
      layout.textBoxBounds = localBounds;
    } else {
      layout.textBoxBounds.setWidth(textBoxWidth);
      layout.textBoxBounds.setHeight(textBoxHeight);

      if (textBoxPos == juce::Slider::TextBoxLeft)
        layout.textBoxBounds.setX(0);
      else if (textBoxPos == juce::Slider::TextBoxRight)
        layout.textBoxBounds.setX(localBounds.getWidth() - textBoxWidth);
      else /* above or below -> centre horizontally */
        layout.textBoxBounds.setX((localBounds.getWidth() - textBoxWidth) / 2);

      if (textBoxPos == juce::Slider::TextBoxAbove)
        layout.textBoxBounds.setY(0);
      else if (textBoxPos == juce::Slider::TextBoxBelow)
        layout.textBoxBounds.setY(localBounds.getHeight() - textBoxHeight);
      else /* left or right -> centre vertically */
        layout.textBoxBounds.setY((localBounds.getHeight() - textBoxHeight) /
                                  2);
    }
  }

  // set the slider bounds
  layout.sliderBounds = localBounds;
  if (slider.isBar()) {
    layout.sliderBounds.reduce(1, 1);  // bar border
  } else {
    if (textBoxPos == juce::Slider::TextBoxLeft)
      layout.sliderBounds.removeFromLeft(textBoxWidth);
    else if (textBoxPos == juce::Slider::TextBoxRight)
      layout.sliderBounds.removeFromRight(textBoxWidth);
    else if (textBoxPos == juce::Slider::TextBoxAbove)
      layout.sliderBounds.removeFromTop(textBoxHeight);
    else if (textBoxPos == juce::Slider::TextBoxBelow)
      layout.sliderBounds.removeFromBottom(textBoxHeight);
    const int thumbIndent = getSliderThumbRadius(slider);
    if (slider.isHorizontal())
      layout.sliderBounds.reduce(thumbIndent, 0);
    else if (slider.isVertical())
      layout.sliderBounds.reduce(0, thumbIndent);
  }
  return layout;
}
