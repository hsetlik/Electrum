#include "EnvelopeGraph.h"
EnvelopeGraph::EnvelopeGraph(EVT* tree, int idx) :
state(tree),
index(idx),
attackEnd(this),
holdEnd(this),
decayEnd(this),
sustainEnd(this),
selectedPoint(nullptr),
isMoving(false)
{
  String iStr(index);
  String atkId = IDs::attackMs.toString() + iStr;

  attackAttach.reset(new DragPointAttachment(
    state,
    atkId,
    &attackEnd,
    [this, atkId](Point<float> pt)
    {
      return getParamFromPos(atkId, &attackEnd, pt);
    },
    [this, atkId](float val)
    {
      return getPosFromParam(atkId, &attackEnd, val);
    }
  ));
  
  String holdId = IDs::holdMs.toString() + iStr;
  holdAttach.reset(new DragPointAttachment(
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
  decayAttach.reset(new DragPointAttachment(
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
  releaseAttach.reset(new DragPointAttachment(
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
   drawEnvelopeGraph(fBounds, g);
}

void EnvelopeGraph::handleAsyncUpdate()
{
  repaint();
}

//======================================================================================
void EnvelopeGraph::drawEnvelopeGraph(Rectangle<float>& bounds, Graphics& g)
{ 
  Path p;
  p.startNewSubPath(bounds.getX(), bounds.getY());
  p.lineTo(attackEnd.getX(), attackEnd.getY());
  p.lineTo(holdEnd.getX(), holdEnd.getY());
  p.lineTo(decayEnd.getX(), decayEnd.getY());
  p.lineTo(sustainEnd.getX(), sustainEnd.getY());
  g.setColour(Color::brightYellow);
  PathStrokeType pst(1.0f);
  g.strokePath(p, pst); }

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
  }
  else
    isMoving = false;
}

void EnvelopeGraph::mouseUp(const MouseEvent& e) 
{
  if (selectedPoint != nullptr)
  {
    selectedPoint->endMove();
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
    auto xAtk = range.convertTo0to1(value) * getMaxAttackLength(fBounds);
    return {xAtk, yTop};
  }
  else if(paramID.contains(IDs::holdMs.toString()))
  {
    auto range = state->getAPVTS()->getParameterRange(paramID);
    // we need to offset this by the attack x value
    auto xHold = (range.convertTo0to1(value) * getMaxHoldLength(fBounds)) + attackEnd.getX();
    return {xHold, yTop};
  }
  else if(paramID.contains(IDs::decayMs.toString()))
  {
    // since sustain hasn't been passed to this function, the y positions for the last two points just keep their existing values
    auto range = state->getAPVTS()->getParameterRange(paramID);
    auto xDecay = (range.covertTo0to1(value) * getMaxDecayLength(fBounds)) + attackEnd.getX() + holdEnd.getX();
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
    auto xRelease = fBounds.getRight() - (range.convertTo0to1(value) * getMaxReleaseLength(fBounds));
    return {xRelease, point->getY()};
  }
}

float EnvelopeGraph::getParamFromPos(const String& paramID, DragPoint* point, Point<float> pos)
{
  auto range = state->getAPVTS()->getParameterRange(paramID);
  auto fBounds = getLocalBounds().toFloat();
  if(point == &attackEnd)
  {
    auto fAttack = (pos.x / getMaxAttackLength(fBounds));
    return range.convertFrom0to1(fAttack);
  }
  else if (point == &holdEnd)
  {
    auto fHold = (pos.x - attackEnd.getX()) / getMaxHoldLength(fBounds);
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
      auto fDecay = (pos.x - attackEnd.getX() - holdEnd.getX()) / getMaxDecayLength(fBounds);
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
  else if (pt == &holdEnd)
  {
    auto xMin = attackEnd.getX();
    return {xMin, yTop, getMaxHoldLength(fBounds), 0.0f};
  }
  else if(pt == &decayEnd)
  {
    auto xMin = attackEnd.getX() + holdEnd.getX();
    return {xMin, yTop, getMaxDecayLength(fBounds), yHeight};
  }
  else //sustainEnd
  {
    auto width = getMaxReleaseLength(fBounds);
    return {fBounds.getRight() - width, yTop, width, yHeight};
  }
}
