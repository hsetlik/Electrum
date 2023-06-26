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
  g.strokePath(p, pst);
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
  selectedPoint  nullptr;
}

//============================================================

void EnvelopeGraph::updateDragPointPositions()
{
  auto bounds = getLocalBounds().toFloat();
  const float yTop = bounds.getY() + 7.0f;
  static frange attackRange = IDs::getAttackRange();
  static frange holdRange = IDs::getHoldRange();
  static frange decayRange = IDs::getDecayRange();
  static frange releaseRange = IDs::getReleaseRange();
  const float aMax = ((float)ATTACK_MS_MAX / (float)ENV_MS_MAX) * bounds.getWidth();
  const float hMax = ((float)HOLD_MS_MAX / (float)ENV_MS_MAX) * bounds.getWidth();
  const float dMax = ((float)DECAY_MS_MAX / (float)ENV_MS_MAX) * bounds.getWidth();
  const float rMax = ((float)RELEASE_MS_MAX / (float)ENV_MS_MAX) * bounds.getWidth(); 
 

}

Point<float> EnvelopeGraph::getPosFromParam(const String& paramID, DragPoint* point, float value)
{
  if(paramID.contains(IDs::attackMs.toString()))
  {
    
  }
  else if(paramID.contains(IDs::holdMs.toString()))
  {

  }
  else if(paramID.contains(IDs::decayMs.toString()))
  {

  }
  else if(paramID.contains(IDs::sustainLevel.toString()))
  {

  }
  else // releaseMs
  {

  }
}

float EnvelopeGraph::getParamFromPos(const String& paramID, DragPoint* point, Point<float> pos)
{
  if(point == &attackEnd)
  {

  }
  else if (point == &holdEnd)
  {

  }
  else if(point == &decayEnd)
  {

  }
  else //sustainEnd
  {
    
  }
}

Point<float> EnvelopeGraph::constrainPositionFor(DragPoint* point, Point<float> pos)
{
  return pos;
  //TODO:
  if(point == &attackEnd)
  {

  }
  else if (point == &holdEnd)
  {

  }
  else if(point == &decayEnd)
  {

  }
  else //sustainEnd
  {
    
  }
}

DragPoint* EnvelopeGraph::getPointWithinRadius(const MouseEvent& e, float radius)
{
  for(auto p : points)
  {
    if(p->isWithin(e, radius))
      return p;
  }
}
