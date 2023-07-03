#include "EnvelopeGraph.h"
EnvelopeGraph::EnvelopeGraph(EVT* tree, int idx) :
state(tree),
index(idx),
attackEnd(this),
attackCurve(this),
holdEnd(this),
decayCurve(this),
decayEnd(this),
sustainEnd(this),
releaseCurve(this),
selectedPoint(nullptr),
isMoving(false)
{
  String iStr(index);
  String atkMsID = IDs::attackMs.toString() + iStr;

  attackMsAttach.reset(new DragPointAttachment(
    state,
    atkMsID,
    &attackEnd,
    [this, atkMsID](Point<float> pt)
    {
      return getParamFromPos(atkMsID, &attackEnd, pt);
    },
    [this, atkMsID](float val)
    {
      return getPosFromParam(atkMsID, &attackEnd, val);
    }
  ));

  String atkCurveID = IDs::attackCurve.toString() + iStr;
  attackCurveAttach.reset(new DragPointAttachment(
    state,
    atkCurveID,
    &attackCurve,
    [this, atkCurveID](Point<float> pt)
    {
      return getParamFromPos(atkCurveID, &attackCurve, pt);
    },
    [this, atkCurveID](float val)
    {
      return getPosFromParam(atkCurveID, &attackCurve, val);
    }
  ));


  
  String holdId = IDs::holdMs.toString() + iStr;
  holdMsAttach.reset(new DragPointAttachment(
    state,
    holdId,
    &holdEnd,
    [this, holdId](Point<float> pt)
    {
      return getParamFromPos(holdId, &holdEnd, pt);
    },
    [this, holdId](float val)
    {
      return getPosFromParam(holdId, &holdEnd, val);
    }
  ));

  String decayId = IDs::decayMs.toString() + iStr;
  decayMsAttach.reset(new DragPointAttachment(
    state,
    decayId,
    &decayEnd,
    [this, decayId](Point<float> pt)
    {
      return getParamFromPos(decayId, &decayEnd, pt);
    },
    [this, decayId](float val)
    {
      return getPosFromParam(decayId, &decayEnd, val);
    }
  ));
  
  String sustainId = IDs::sustainLevel.toString() + iStr;
  sustainAttach1.reset(new DragPointAttachment(
    state,
    sustainId,
    &decayEnd,
    [this, sustainId](Point<float> pt)
    {
      return getParamFromPos(sustainId, &decayEnd, pt);
    },
    [this, sustainId](float val)
    {
      return getPosFromParam(sustainId, &decayEnd, val);
    }
  ));
  sustainAttach2.reset(new DragPointAttachment(
    state,
    sustainId,
    &sustainEnd,
    [this, sustainId](Point<float> pt)
    {
      return getParamFromPos(sustainId, &sustainEnd, pt);
    },
    [this, sustainId](float val)
    {
      return getPosFromParam(sustainId, &sustainEnd, val);
    }
  )); 
  
  String releaseId = IDs::releaseMs.toString() + iStr;
  releaseMsAttach.reset(new DragPointAttachment(
    state,
    releaseId,
    &sustainEnd,
    [this, releaseId](Point<float> pt)
    {
      return getParamFromPos(releaseId, &sustainEnd, pt);
    },
    [this, releaseId](float val)
    {
      return getPosFromParam(releaseId, &sustainEnd, val);
    }
  )); 
  triggerAsyncUpdate();
}

void EnvelopeGraph::paint(Graphics& g)
{
   auto fBounds = getLocalBounds().toFloat();
  //  g.setColour(Color::aquamarine);
  //  g.fillRect(getLimitsFor(&attackEnd));
  //  g.setColour(Color::lightSteelBlue);
  //  g.fillRect(getLimitsFor(&holdEnd));
  //  g.setColour(Color::chocolate);
  //  g.fillRect(getLimitsFor(&decayEnd));
  //  g.setColour(Color::chartreuse);
  //  g.fillRect(getLimitsFor(&sustainEnd));
   drawEnvelopeGraph(fBounds, g);
}

void EnvelopeGraph::handleAsyncUpdate()
{
  syncWithState();
  repaint();
}

//======================================================================================
void EnvelopeGraph::drawEnvelopeGraph(Rectangle<float>& bounds, Graphics& g)
{ 
  const int curvePoints = 60;
  //draw the path
  Path p;
  p.startNewSubPath(bounds.getX(), bounds.getBottom());
  // draw the attack curve
  Path p2;
  p2.startNewSubPath(bounds.getX(), bounds.getBottom());
  const float yMax = bounds.getHeight() - 5.0f;
  for(int i = 0; i < curvePoints; i++)
  {
    float t = ((float)i / (float)curvePoints);
    float fX = t * attackEnd.getX();
    float fY = bounds.getBottom() - 5.0f - Math::onEasingCurve(0.0f, yMax - attackCurve.getY(), yMax, t);
    fY = std::max({fY, 5.0f});
    p.lineTo(fX, fY);
  }
  p.lineTo(holdEnd.getX(), holdEnd.getY());
  p.lineTo(decayEnd.getX(), decayEnd.getY());
  p.lineTo(sustainEnd.getX(), sustainEnd.getY());
  p.lineTo(bounds.getRight(), bounds.getBottom());
  g.setColour(Color::brightYellow);
  PathStrokeType pst(1.2f);
  g.strokePath(p, pst);

  // draw the handles
  drawHandle(g, attackEnd.getPos(), 3.0f, selectedPoint != &attackEnd);
  drawHandle(g, attackCurve.getPos(), 3.0f, selectedPoint != &attackCurve);
  drawHandle(g, holdEnd.getPos(), 3.0f, selectedPoint != &holdEnd);
  drawHandle(g, decayEnd.getPos(), 3.0f, selectedPoint != &decayEnd);
  drawHandle(g, sustainEnd.getPos(), 3.0f, selectedPoint != &sustainEnd);
}

void EnvelopeGraph::mouseDown(const MouseEvent& e) 
{
  for(auto* p : points)
  {
    if(p->isWithin(e, 3.0f))
    {
      selectedPoint = p;
      return;
    }
  }
  selectedPoint = nullptr;
  isMoving = false;
}

void EnvelopeGraph::mouseDrag(const MouseEvent& e) 
{
  if(selectedPoint != nullptr)
  {
    if(!isMoving)
    {
      isMoving = true;
      selectedPoint->startMove();
    }
    auto destPos = constrainPositionFor(selectedPoint, e.position);
    selectedPoint->movePoint(destPos.x, destPos.y);
    triggerAsyncUpdate();
  }
  else
    isMoving = false;
}

void EnvelopeGraph::mouseUp(const MouseEvent&) 
{
  if (selectedPoint != nullptr)
  {
    selectedPoint->endMove();
    triggerAsyncUpdate();
  }
  selectedPoint = nullptr;
}

//============================================================

Point<float> EnvelopeGraph::getPosFromParam(const String& paramID, DragPoint* point, float value)
{
  auto fBounds = getLocalBounds().toFloat();
  const float yTop = fBounds.getY() + 5.0f;

  if(paramID.contains(IDs::attackMs.toString()))
  {
    auto range = state->getAPVTS()->getParameterRange(paramID);
    if(!range.getRange().contains(value))
    {
      DLog::log("Out of range attack value: " + String(value));
    }
    auto xAtk = range.convertTo0to1(value) * getMaxAttackLength(fBounds);
    return {xAtk, yTop};
  }
  else if(paramID.contains(IDs::attackCurve.toString()))
  {
    float x0 = 0.0f;
    float y0 = yTop;
    float x1 = attackEnd.getX();
    float y1 = fBounds.getBottom() - 5.0f;
    float xPos = Math::flerp(x0, x1, 0.5f);
    float yPos = Math::flerp(y1, y0, value);
    return {xPos, yPos};
  }
  else if(paramID.contains(IDs::holdMs.toString()))
  {
    auto range = state->getAPVTS()->getParameterRange(paramID);
    if(!range.getRange().contains(value))
    {
      //DLog::log("Out of range hold value: " + String(value));
    }
    // we need to offset this by the attack x value
    auto xHold = (range.convertTo0to1(value) * getMaxHoldLength(fBounds)) + attackEnd.getX();
    return {xHold, yTop};
  }
  else if(paramID.contains(IDs::decayMs.toString()))
  {
    // since sustain hasn't been passed to this function, the y positions for the last two points just keep their existing values
    auto range = state->getAPVTS()->getParameterRange(paramID);
    if(!range.getRange().contains(value))
    {
      DLog::log("Out of range decay value: " + String(value));
    }
    auto xDecay = (range.convertTo0to1(value) * getMaxDecayLength(fBounds)) + holdEnd.getX();
    return {xDecay, point->getY()};
  }
  else if(paramID.contains(IDs::sustainLevel.toString()))
  {
    // since this could be one of two points, we keep the point's previous X value
    auto susY = sustainLevelY(fBounds, value);
    return {point->getX(), susY};
  }
  else // releaseMs
  {
    auto range = state->getAPVTS()->getParameterRange(paramID);
    if(!range.getRange().contains(value))
    {
      DLog::log("Out of range release value: " + String(value));
    }
    auto xRelease = fBounds.getRight() - (range.convertTo0to1(value) * getMaxReleaseLength(fBounds));
    return {xRelease, point->getY()};
  }
}

float EnvelopeGraph::getParamFromPos(const String& paramID, DragPoint* point, Point<float> pos)
{
  auto range = state->getAPVTS()->getParameterRange(paramID);
  auto fBounds = getLocalBounds().toFloat();
  const float yTop = fBounds.getY() + 5.0f;
  if(point == &attackEnd)
  {
    auto fAttack = (pos.x / getMaxAttackLength(fBounds));
    if(fAttack < 0.0f || fAttack > 1.0f)
    {
      DLog::log("FAttack is out of range in EnvelopeGraph.cpp!");
    }
    return range.convertFrom0to1(fAttack);
    
  }
  else if (point == &attackCurve)
  {
    float y0 = yTop;
    float y1 = fBounds.getBottom() - 5.0f;
    return (pos.y - y1) / (y0 - y1);
  }
  else if (point == &holdEnd)
  {
    auto fHold = (pos.x - attackEnd.getX()) / getMaxHoldLength(fBounds);
    if(fHold < 0.0f || fHold > 1.0f)
    {
      DLog::log("fHold is out of range in EnvelopeGraph.cpp!");
    }
    return range.convertFrom0to1(fHold);
  }
  else if(point == &decayEnd) //careful: what looks like two cases is actually four cases because the last two points can control 1 of 2 parameters
  {
    if(paramID.contains(IDs::sustainLevel.toString())) // sustain
    {
      return sustainFromY(fBounds, pos.y); 
    }
    else // decay time
    {
      auto fDecay = (pos.x - holdEnd.getX()) / getMaxDecayLength(fBounds);
      if(fDecay < 0.0f || fDecay > 1.0f)
      {
        DLog::log("fDecay is out of range in EnvelopeGraph.cpp!");
      }
      return range.convertFrom0to1(fDecay);
    }
  }
  else //sustainEnd
  {
    if(paramID.contains(IDs::sustainLevel.toString())) // sustain
    {
      return sustainFromY(fBounds, pos.y); 
    }
    else // release time
    {
      auto fRelease = (fBounds.getRight() - pos.x) / getMaxReleaseLength(fBounds);
      if(fRelease < 0.0f || fRelease > 1.0f)
      {
        DLog::log("fRelease is out of range in EnvelopeGraph.cpp!");
      }
      return range.convertFrom0to1(fRelease);
    }
  }
}

Point<float> EnvelopeGraph::constrainPositionFor(DragPoint* point, Point<float> pos)
{
  auto bounds = getLimitsFor(point);
  return bounds.getConstrainedPoint(pos);
}

DragPoint* EnvelopeGraph::getPointWithinRadius(const MouseEvent& e, float radius)
{
  for(auto p : points)
  {
    if(p->isWithin(e, radius))
      return p;
  }
  return nullptr;
}

Rectangle<float> EnvelopeGraph::getLimitsFor(DragPoint* pt)
{
  auto fBounds = getLocalBounds().toFloat();
  auto yTop = fBounds.getY() + 5.0f;
  auto yBottom = fBounds.getBottom() - 5.0f;
  float yHeight = yBottom - yTop;
  if(pt == &attackEnd)
  {
    return {0.0f, yTop, getMaxAttackLength(fBounds), 0.0f};
  }
  else if(pt == &attackCurve)
  {
    auto cX = attackEnd.getX() / 2.0f;
    return {cX, yTop, 0.0f, yHeight};
  }
  else if (pt == &holdEnd)
  {
    auto xMin = attackEnd.getX();
    return {xMin, yTop, getMaxHoldLength(fBounds), 0.0f};
  }
  else if(pt == &decayEnd)
  {
    auto xMin = holdEnd.getX();
    return {xMin, yTop, getMaxDecayLength(fBounds), yHeight};
  }
  else //sustainEnd
  {
    auto width = getMaxReleaseLength(fBounds);
    return {fBounds.getRight() - width, yTop, width, yHeight};
  }
}

void EnvelopeGraph::syncWithState()
{
  String iStr(index);
  auto attackMsID = IDs::attackMs.toString() + iStr;
  auto attackCurveID = IDs::attackCurve.toString() + iStr;
  auto holdID = IDs::holdMs.toString() + iStr;
  auto decayID = IDs::decayMs.toString() + iStr;
  auto sustainID = IDs::sustainLevel.toString() + iStr;
  auto releaseID = IDs::releaseMs.toString() + iStr;

  const float attackMs = state->getFloatParamValue(attackMsID);
  const float attackCurveValue = state->getFloatParamValue(attackCurveID);
  const float holdMs = state->getFloatParamValue(holdID);
  const float decayMs = state->getFloatParamValue(decayID);
  const float sustainLevel = state->getFloatParamValue(sustainID);
  const float releaseMs = state->getFloatParamValue(releaseID);
// check each parameter, update if it's changed
  auto newAtkPos = getPosFromParam(attackMsID, &attackEnd, attackMs); 
  if(newAtkPos != attackEnd.getPos())
  {
    //DLog::log("Updating attack position");
    attackEnd.moveTo(newAtkPos);
  }

  auto aCurvePos = getPosFromParam(attackCurveID, &attackCurve, attackCurveValue);
  if(aCurvePos != attackCurve.getPos())
  {
    attackCurve.moveTo(aCurvePos);
  }

  auto holdPos = getPosFromParam(holdID, &holdEnd, holdMs); 
  if(holdPos != holdEnd.getPos())
  {
    //DLog::log("Updating hold position");
    holdEnd.moveTo(holdPos);
  }
  auto sustainPos = getPosFromParam(sustainID, &decayEnd, sustainLevel);
  auto decayPos = getPosFromParam(decayID, &decayEnd, decayMs);
  decayPos.y = sustainPos.y;
  if(decayPos != decayEnd.getPos()) // decay time has changed
  {
    //DLog::log("Updating decay position");
    decayEnd.moveTo(decayPos);
  }

  auto releasePos = getPosFromParam(releaseID, &sustainEnd, releaseMs);
  releasePos.y = sustainPos.y;
  if(releasePos != sustainEnd.getPos())
  {
    //DLog::log("Updating release position");
    sustainEnd.moveTo(releasePos);
  }
}

void EnvelopeGraph::drawHandle(Graphics& g, Point<float> center, float radius, bool fill)
{
  Rectangle<float> bounds(radius * 2.0f, radius * 2.0f);
  bounds = bounds.withCentre(center);
  g.setColour(Color::brightYellow);
  if(fill)
  {
    g.fillEllipse(bounds);
  }
  else
  {
    g.drawEllipse(bounds, HANDLE_STROKE);
  }
}