#include "Electrum/GUI/Modulation/WedgeButton.h"
#include "juce_core/juce_core.h"

juce::Path WedgeButton::getWedgePath(float x0,
                                     float y0,
                                     float startRads,
                                     float endRads,
                                     float r1,
                                     float r2,
                                     float offsetX,
                                     float offsetY) {
  juce::Point<float> center(x0, y0);

  // these juce::Point methods treat 0 radians as 12 o'clock
  // for some reason so I offset both by pi/2 here
  startRads += juce::MathConstants<float>::halfPi;
  endRads += juce::MathConstants<float>::halfPi;
  // start & end of the inner arc
  juce::Point<float> start1 = center.getPointOnCircumference(r1, startRads);
  juce::Point<float> end1 = center.getPointOnCircumference(r1, endRads);

  juce::Point<float> start2 = center.getPointOnCircumference(r2, startRads);
  juce::Point<float> end2 = center.getPointOnCircumference(r2, endRads);

  juce::Path out;
  out.startNewSubPath(start1);
  // draw the inner arc from startRads to endRads
  for (int i = 0; i < WEDGE_SEGMENTS; ++i) {
    float t = (float)i / (float)WEDGE_SEGMENTS;
    float angle = flerp(startRads, endRads, t);
    out.lineTo(center.getPointOnCircumference(r1, angle));
  }
  out.lineTo(end1);
  out.lineTo(end2);
  // now go backwards for the outer arc
  for (int i = WEDGE_SEGMENTS - 1; i >= 0; --i) {
    float t = (float)i / (float)WEDGE_SEGMENTS;
    float angle = flerp(startRads, endRads, t);
    out.lineTo(center.getPointOnCircumference(r2, angle));
  }
  out.lineTo(start2);
  out.closeSubPath();
  if (offsetX != 0.0f || offsetY != 0.0f) {
    out.applyTransform(
        juce::AffineTransform::translation(offsetX * -1.0f, offsetY * -1.0f));
  }
  return out;
}

juce::Rectangle<float> WedgeButton::getWedgeBounds(float x0,
                                                   float y0,
                                                   float startRads,
                                                   float endRads,
                                                   float r1,
                                                   float r2) {
  auto path = getWedgePath(x0, y0, startRads, endRads, r1, r2);
  return path.getBounds();
}
//====================================================================================================================

WedgeButton::WedgeButton(const String& name,
                         float startRads,
                         float endRads,
                         float rad1,
                         float rad2)
    : Button(name), a1(startRads), a2(endRads), r1(rad1), r2(rad2) {}

void WedgeButton::centerOn(Component* parent) {
  auto pBounds = parent->getLocalBounds().toFloat();
  auto bounds = getWedgeBounds(pBounds.getCentreX(), pBounds.getCentreY(), a1,
                               a2, r1, r2);
  setBounds(bounds.toNearestInt());
}
//===================================================
