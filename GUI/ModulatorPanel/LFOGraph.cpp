#include "LFOGraph.h"

LFOGraph::LFOGraph(EVT *tree, int i)
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
  triggerAsyncUpdate();
}

DragPoint *LFOGraph::getPointWithinRadius(const MouseEvent &e, float radius)
{
  for (auto p : points)
  {
    if (p->isWithin(e, radius))
      return p;
  }
  return nullptr;
}

Point<float> LFOGraph::getPosFromParam(const String &paramID, DragPoint *p, float value)
{
  auto range = state->getAPVTS()->getParameterRange(paramID);
  if (!range.getRange().contains(value))
  {
    // DLog::log("Warning: out of range parameter " + paramID);
    value = range.snapToLegalValue(value);
  }
  float vNorm = range.convertTo0to1(value);
  auto limits = getLimitsFor(p);
  if (p == &center)
  {
    float yPos = limits.getY();
    float xPos = Math::flerp(limits.getX(), limits.getRight(), vNorm);
    return {xPos, yPos};
  } else
  {
    float xPos = Math::flerp(limits.getX(), limits.getRight(), 0.5f);
    float yPos = Math::flerp(limits.getY(), limits.getBottom(), vNorm);
    return {xPos, yPos};
  }
}

float LFOGraph::getParamFromPos(const String &paramID, DragPoint *point, Point<float> pos)
{
  auto range = state->getAPVTS()->getParameterRange(paramID);
  auto limits = getLimitsFor(point);
  if (!limits.contains(pos))
  {
    // DLog::log("Warning: Point for " + paramID + " is out of limits!");
    pos = limits.getConstrainedPoint(pos);
  }
  if (point == &center)
  {
    float vNorm = (pos.x - limits.getX()) / limits.getWidth();
    jassert(vNorm >= 0.0f && vNorm <= 1.0f);
    return range.convertFrom0to1(vNorm);
  } else
  {
    float vNorm = (pos.y - limits.getY()) / limits.getHeight();
    jassert(vNorm >= 0.0f && vNorm <= 1.0f);
    return range.convertFrom0to1(vNorm);
  }
}

Point<float> LFOGraph::constrainPositionFor(DragPoint *point, Point<float> pos)
{
  auto bounds = getLimitsFor(point);
  return bounds.getConstrainedPoint(pos);
}

Rectangle<float> LFOGraph::getLimitsFor(DragPoint *pt)
{
  auto fBounds = getLocalBounds().toFloat();
  auto yTop = fBounds.getY() + 5.0f;
  auto yBottom = fBounds.getBottom() - 5.0f;
  float yHeight = yBottom - yTop;
  const float xMin = 1.0f;
  const float centerMin = 2.5f;
  if (pt == &curveA)
  {
    float width = (center.getX() - xMin) - xMin;
    return {xMin, yTop, width, yHeight};
  } else if (pt == &center)
  {
    return {centerMin, yTop, fBounds.getWidth() - (centerMin * 2.0f), 0.0f};
  } else
  {
    float width = (fBounds.getRight() - xMin) - (center.getX() + xMin);
    return {center.getX() + xMin, yTop, width, yHeight};
  }
}

void LFOGraph::syncWithState()
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
//============================================================================================

void LFOGraph::mouseDown(const MouseEvent &e)
{
  for (auto *p : points)
  {
    if (p->isWithin(e, 3.0f))
    {
      selectedPoint = p;
      return;
    }
  }
  selectedPoint = nullptr;
  isMoving = false;
}

void LFOGraph::mouseDrag(const MouseEvent &e)
{
  if (selectedPoint != nullptr)
  {
    if (!isMoving)
    {
      isMoving = true;
      selectedPoint->startMove();
    }
    auto destPos = constrainPositionFor(selectedPoint, e.position);
    selectedPoint->movePoint(destPos.x, destPos.y);
    triggerAsyncUpdate();
  } else
    isMoving = false;
}

void LFOGraph::mouseUp(const MouseEvent &)
{
  if (selectedPoint != nullptr)
  {
    selectedPoint->endMove();
    triggerAsyncUpdate();
  }
  selectedPoint = nullptr;
}

void LFOGraph::paint(Graphics &g)
{
  auto fBounds = getLocalBounds().toFloat().reduced(2.0f);
  drawLFOGraph(fBounds, g);
}

void LFOGraph::handleAsyncUpdate()
{
  syncWithState();
  repaint();
}

//============================================================================================
void LFOGraph::drawHandle(Graphics &g, Point<float> center, float radius, bool fill)
{
  Rectangle<float> bounds(radius * 2.0f, radius * 2.0f);
  bounds = bounds.withCentre(center);
  g.setColour(Color::brightSeafoam);
  if (fill)
  {
    g.fillEllipse(bounds);
  } else
  {
    g.drawEllipse(bounds, HANDLE_STROKE);
  }
}

void LFOGraph::drawLFOGraph(Rectangle<float> &bounds, Graphics &g)
{
  const int curvePoints = 60;
  const float yMax = bounds.getBottom() - 3.0f;
  if (bounds.getHeight() < 1.0f || bounds.getWidth() < 1.0f)
  {
    return;
  }
  // draw the path
  Path p;
  p.startNewSubPath(bounds.getX(), bounds.getBottom());
  for (int i = 0; i < curvePoints; i++)
  {
    float t = ((float)i / (float)curvePoints);
    float fX = t * center.getX();
    float fY = yMax - Math::onEasingCurve(0.0f, yMax - curveA.getY(), yMax - center.getY(), t);
    p.lineTo(fX, fY);
  }
  p.lineTo(center.getX(), center.getY());
  for (int i = 0; i < curvePoints; i++)
  {
    float t = ((float)i / (float)curvePoints);
    float fX = Math::flerp(center.getX(), bounds.getRight(), t);
    float fY =
        yMax - Math::onEasingCurve(0.0f, yMax - curveB.getY(), yMax - center.getY(), 1.0f - t);
    p.lineTo(fX, fY);
  }
  p.lineTo(bounds.getRight(), bounds.getBottom());
  g.setColour(Color::brightSeafoam);
  PathStrokeType pst(1.2f);
  g.strokePath(p, pst);

  drawHandle(g, curveA.getPos(), 3.0f, selectedPoint != &curveA);
  drawHandle(g, center.getPos(), 3.0f, selectedPoint != &center);
  drawHandle(g, curveB.getPos(), 3.0f, selectedPoint != &curveB);
}
//============================================================================================
LFOPanel::LFOPanel(EVT *tree, int i)
    : state(tree), index(i), graph(tree, i),
      freqSlider(tree, IDs::lfoFreq.toString() + String(i), "Freq.", true),
      srcComp(tree, IDs::lfoSource.toString() + String(i))
{
  addAndMakeVisible(&graph);
  addAndMakeVisible(&srcComp);
  addAndMakeVisible(&freqSlider);
}

void LFOPanel::resized()
{
  auto fBounds = getLocalBounds().toFloat();
  auto sliderArea = fBounds.removeFromLeft(fBounds.getWidth() / 6.0f);
  auto srcArea = sliderArea.removeFromTop(sliderArea.getWidth());
  srcComp.setBounds(srcArea.toNearestInt());
  freqSlider.setBounds(sliderArea.toNearestInt());
  graph.setBounds(fBounds.toNearestInt());
}
