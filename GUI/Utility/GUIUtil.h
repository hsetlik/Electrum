#pragma once
#ifndef GUIUTIL_H
#define GUIUTIL_H
#include "../../Parameters/DLog.h"
#include "../../Parameters/MathUtil.h"
namespace GUIUtil {
inline void strokeArc(Graphics &g, float x, float y, float w, float h, float startRads,
                      float endRads, float lineWeight)
{
  Path path;
  path.addArc(x, y, w, h, startRads, endRads, true);
  PathStrokeType pst(lineWeight, PathStrokeType::JointStyle::mitered,
                     PathStrokeType::EndCapStyle::rounded);
  g.strokePath(path, pst);
}
inline void strokeArc(Graphics &g, Rectangle<float> area, float startRads, float endRads,
                      float lineWeight)
{
  strokeArc(g, area.getX(), area.getY(), area.getWidth(), area.getHeight(), startRads, endRads,
            lineWeight);
}

inline AffineTransform getTransformForSlider(Rectangle<float> &area, float startRads, float endRads,
                                             float sliderPos)
{
  auto angle = Math::flerp(startRads, endRads, sliderPos);
  float pivotX = area.getX() + (area.getWidth() / 2.0f);
  float pivotY = area.getY() + (area.getHeight() / 2.0f);
  return AffineTransform::rotation(angle, pivotX, pivotY);
}

// this is just for debugging, checks if something is wrong with the point and describes the issue
inline void checkPointIsValid(Point<float> &pt, Rectangle<float> &bounds)
{
  bool invalidNums = false;
  // first check if we just got NaN values
  if (std::isnan(pt.x))
  {
    DLog::log("X is NaN");
    invalidNums = true;
  }
  if (std::isnan(pt.y))
  {
    DLog::log("Y is NaN");
    invalidNums = true;
  }
  // don't bother checking the range if we have NaN values
  if (invalidNums)
    return;
  // now check that the point is within the rectangle
  if (pt.x < bounds.getX())
    DLog::log("Point X at " + String(pt.x) + " is left of limit " + String(bounds.getX()));
  if (pt.y < bounds.getY())
    DLog::log("Point Y at " + String(pt.y) + " is above limit " + String(bounds.getY()));
  if (pt.x > bounds.getRight())
    DLog::log("Point X at " + String(pt.x) + " is right of limit " + String(bounds.getRight()));
  if (pt.y > bounds.getBottom())
    DLog::log("Point Y at " + String(pt.y) + " is below limit " + String(bounds.getBottom()));
}

} // namespace GUIUtil

#endif
