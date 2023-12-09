#include "ElectrumLookAndFeel.h"

void ElectrumLookAndFeel::drawRotarySlider(Graphics &g, int x, int y, int width,
                                           int height,
                                           float sliderPosProportional,
                                           float rotaryStartAngle,
                                           float rotaryEndAngle, Slider &)
{
  float centerX = (float)x + ((float)width / 2.0f);
  float centerY = (float)y + ((float)height / 2.0f);
  float rOuter = (float)width / 2.0f;
  float rInner = rOuter * 0.75f;
  Path p;
  p.addCentredArc(centerX, centerY, rInner, rInner, 0.0f, rotaryStartAngle,
                  rotaryEndAngle, true);
  p.addCentredArc(centerX, centerY, rOuter * 0.85f, rOuter * 0.85f, 0.0f,
                  rotaryEndAngle, rotaryStartAngle, false);
  p.closeSubPath();
  g.setColour(Color::dimGray);
  g.fillPath(p);
  const float thumbAngleWidth = 0.3f;
  float thumbCenterAngle =
      Math::flerp(rotaryStartAngle, rotaryEndAngle, sliderPosProportional);
  Path thumb;
  thumb.addCentredArc(centerX, centerY, rOuter, rOuter, 0.0f,
                      thumbCenterAngle - thumbAngleWidth,
                      thumbCenterAngle + thumbAngleWidth, true);
  thumb.addCentredArc(centerX, centerY, rInner * 0.85f, rInner * 0.85f, 0.0f,
                      thumbCenterAngle + thumbAngleWidth,
                      thumbCenterAngle - thumbAngleWidth, false);
  thumb.closeSubPath();
  g.setColour(Color::darkCyan);
  g.fillPath(thumb);
}
