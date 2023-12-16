#include "ElectrumLookAndFeel.h"
#include "FontBinaries.h"
#include "Fonts.h"

ElectrumLookAndFeel::ElectrumLookAndFeel()
    : labelFont(Fonts::getTypeface(Fonts::FuturaLightCondensed))
{
  setDefaultSansSerifTypeface(Fonts::getTypeface(Fonts::FuturaLightCondensed));
}
void ElectrumLookAndFeel::drawRotarySlider(Graphics &g, int x, int y, int width, int height,
                                           float sliderPosProportional, float rotaryStartAngle,
                                           float rotaryEndAngle, Slider &)
{
  float centerX = (float)x + ((float)width / 2.0f);
  float centerY = (float)y + ((float)height / 2.0f);
  float rOuter = (float)width / 2.0f;
  float rInner = rOuter * 0.75f;
  Path p;
  p.addCentredArc(centerX, centerY, rInner, rInner, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
  p.addCentredArc(centerX, centerY, rOuter, rOuter, 0.0f, rotaryEndAngle, rotaryStartAngle, false);
  p.closeSubPath();
  auto trackColor = findColour(Slider::ColourIds::trackColourId);
  g.setColour(trackColor);
  g.fillPath(p);
  const float thumbAngleWidth = 0.2f;
  float thumbCenterAngle = Math::flerp(rotaryStartAngle, rotaryEndAngle, sliderPosProportional);
  Path thumb;
  thumb.addCentredArc(centerX, centerY, rOuter, rOuter, 0.0f, thumbCenterAngle - thumbAngleWidth,
                      thumbCenterAngle + thumbAngleWidth, true);
  thumb.addCentredArc(centerX, centerY, rInner * 0.85f, rInner * 0.85f, 0.0f,
                      thumbCenterAngle + thumbAngleWidth, thumbCenterAngle - thumbAngleWidth,
                      false);
  thumb.closeSubPath();
  auto thumbColor = findColour(Slider::ColourIds::thumbColourId);
  g.setColour(thumbColor);
  g.fillPath(thumb);
}

Font ElectrumLookAndFeel::getLabelFont(Label &l)
{
  return labelFont.withHeight((float)(l.getHeight() - l.getBorderSize().getTopAndBottom()));
}

void ElectrumLookAndFeel::drawLabel(Graphics &g, Label &l)
{
  auto f = getLabelFont(l);
  g.setFont(f);
  auto b = l.getBorderSize();
  // fill the background
  g.setColour(findColour(Label::ColourIds::backgroundColourId));
  g.fillRoundedRectangle(l.getLocalBounds().toFloat(), 8.0f);
  // draw the text
  auto textBounds = b.subtractedFrom(l.getLocalBounds());
  g.setColour(findColour(Label::ColourIds::textColourId));
  g.drawFittedText(l.getText(true), textBounds, Justification::centred, 1);
}
