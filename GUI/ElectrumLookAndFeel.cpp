#include "Color.h"
#include "ElectrumLookAndFeel.h"
#include "FontBinaries.h"
#include "Fonts.h"

ElectrumLookAndFeel::ElectrumLookAndFeel()
    : labelFont(Fonts::getTypeface(Fonts::HelveticaNeueMedium))
{
  setDefaultSansSerifTypeface(Fonts::getTypeface(Fonts::HelveticaNeueMedium));
  /* We set colors up here so that we can use the 'ColourIds' in the rest of the code*/
  setColour(Label::backgroundColourId, Color::darkBkgnd);
  setColour(Label::textColourId, Color::offWhite);

  setColour(Slider::backgroundColourId, Color::darkBkgnd);
  setColour(Slider::trackColourId, Color::paleOrange);
  setColour(Slider::thumbColourId, Color::brightSeafoam);
}
void ElectrumLookAndFeel::drawRotarySlider(Graphics &g, int x, int y, int width, int height,
                                           float sliderPosProportional, float rotaryStartAngle,
                                           float rotaryEndAngle, Slider &)
{
  float centerX = (float)x + ((float)width / 2.0f);
  float centerY = (float)y + ((float)height / 2.0f);
  float rOuter = (float)width / 2.0f;
  float rInner = rOuter * 0.75f;
  float thumbCenterAngle = Math::flerp(rotaryStartAngle, rotaryEndAngle, sliderPosProportional);
  Path leftTrack;
  leftTrack.addCentredArc(centerX, centerY, rInner, rInner, 0.0f, rotaryStartAngle,
                          thumbCenterAngle, true);
  leftTrack.addCentredArc(centerX, centerY, rOuter, rOuter, 0.0f, thumbCenterAngle,
                          rotaryStartAngle, false);
  leftTrack.closeSubPath();
  g.setColour(findColour(Slider::ColourIds::trackColourId).darker());
  g.fillPath(leftTrack);

  Path rightTrack;
  rightTrack.addCentredArc(centerX, centerY, rInner, rInner, 0.0f, thumbCenterAngle, rotaryEndAngle,
                           true);
  rightTrack.addCentredArc(centerX, centerY, rOuter, rOuter, 0.0f, rotaryEndAngle, thumbCenterAngle,
                           false);
  rightTrack.closeSubPath();
  g.setColour(findColour(Slider::ColourIds::trackColourId));
  g.fillPath(rightTrack);

  const float thumbAngleWidth = 0.2f;
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
  return l.getFont().withHeight((float)(l.getHeight() - l.getBorderSize().getTopAndBottom()));
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
  g.drawFittedText(l.getText(true), textBounds, l.getJustificationType(), 1);
}
