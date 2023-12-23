#include "LFOGraph.h"

LFOGraphCore::LFOGraphCore(EVT *tree, int i)
    : state(tree), index(i), curveA(this), center(this), curveB(this)
{
  syncWithState();
  String iStr(index);
  String aID = IDs::lfoMidpointA.toString() + iStr;
  String centerID = IDs::lfoCenterX.toString() + iStr;
  String bID = IDs::lfoMidpointB.toString() + iStr;

  curveAAttach.reset(new DragPointAttachment(
      state, aID, &curveA,
      [this, aID](Point<float> pt) { return getParamFromPos(aID, &curveA, pt); },
      [this, aID](float val) { return getPosFromParam(aID, &curveA, val); }));

  centerAttach.reset(new DragPointAttachment(
      state, centerID, &center,
      [this, centerID](Point<float> pt) { return getParamFromPos(centerID, &center, pt); },
      [this, centerID](float val) { return getPosFromParam(centerID, &center, val); }));

  curveBAttach.reset(new DragPointAttachment(
      state, bID, &curveB,
      [this, bID](Point<float> pt) { return getParamFromPos(bID, &curveB, pt); },
      [this, bID](float val) { return getPosFromParam(bID, &curveB, val); }));
}

DragPoint *LFOGraphCore::getPointWithinRadius(const MouseEvent &e, float radius)
{
  for (auto p : points)
  {
    if (p->isWithin(e, radius))
      return p;
  }
  return nullptr;
}

Point<float> LFOGraphCore::getPosFromParam(const String &paramID, DragPoint *, float value)
{
  auto fBounds = getLocalBounds().toFloat();
  const float yTop = fBounds.getY() + 5.0f;
  if (paramID.contains(IDs::lfoCenterX.toString()))
  {
    auto range = state->getAPVTS()->getParameterRange(paramID);
    auto xPos = range.convertTo0to1(value) * fBounds.getWidth();
    return {xPos, yTop};
  } else if (paramID.contains(IDs::lfoMidpointA.toString()))
  {
    float xPos = Math::flerp(0.0f, center.getX(), 0.5f);
    float yPos = Math::flerp(fBounds.getBottom() - 5.0f, yTop, value);
    return {xPos, yPos};
  } else
  {
    float xPos = Math::flerp(center.getX(), fBounds.getWidth(), 0.5f);
    float yPos = Math::flerp(fBounds.getBottom() - 5.0f, yTop, value);
    return {xPos, yPos};
  }
}
float LFOGraphCore::getParamFromPos(const String &paramID, DragPoint *point, Point<float> pos)
{
  auto range = state->getAPVTS()->getParameterRange(paramID);
  auto fBounds = getLocalBounds().toFloat();
  const float yTop = fBounds.getY() + 5.0f;
  if (point == &curveA || point == &curveB)
  {
    float pVal = ((fBounds.getBottom() - 5.0f) - yTop) / pos.y;
    return range.convertFrom0to1(pVal);
  } else
  {
    return range.convertFrom0to1(pos.x / fBounds.getWidth());
  }
}
Point<float> LFOGraphCore::constrainPositionFor(DragPoint *point, Point<float> pos)
{
  auto bounds = getLimitsFor(point);
  return bounds.getConstrainedPoint(pos);
}

Rectangle<float> LFOGraphCore::getLimitsFor(DragPoint *pt)
{
  auto fBounds = getLocalBounds().toFloat();
  auto yTop = fBounds.getY() + 5.0f;
  auto yBottom = fBounds.getBottom() - 5.0f;
  float yHeight = yBottom - yTop;
  const float xMin = 1.0f;
  const float xMax = fBounds.getWidth() - xMin;
  if (pt == &curveA)
  {
    float width = (center.getX() - xMin) - xMin;
    return {xMin, yTop, width, yHeight};
  } else if (pt == &center)
  {
    return {xMin, yTop, fBounds.getWidth() - (xMin * 2.0f), 0.0f};
  } else
  {
    float x = center.getX() + xMin;
    return {x, yTop, xMax - x, yHeight};
  }
}

void LFOGraphCore::syncWithState()
{
  String iStr(index);
  auto centerID = IDs::lfoCenterX.toString() + iStr;
  auto aID = IDs::lfoMidpointA.toString() + iStr;
  auto bID = IDs::lfoMidpointB.toString() + iStr;

  const float centerVal = state->getFloatParamValue(centerID);
  const float aVal = state->getFloatParamValue(aID);
  const float bVal = state->getFloatParamValue(bID);

  auto newCenterPos = getPosFromParam(centerID, &center, centerVal);
  if (newCenterPos != center.getPos())
    center.moveTo(newCenterPos);

  auto newAPos = getPosFromParam(aID, &curveA, aVal);
  if (newAPos != curveA.getPos())
    curveA.moveTo(newAPos);

  auto newBPos = getPosFromParam(bID, &curveB, bVal);
  if (newBPos != curveB.getPos())
    curveB.moveTo(newBPos);
}
