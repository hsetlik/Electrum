#include "EnvelopeGraph.h"

EnvelopeGraph::EnvelopeGraph(EVT* tree, int idx) :
state(tree),
index(idx),
attackEnd(this),
holdEnd(this),
decayEnd(this),
sustainEnd(this),
selectedPoint(nullptr)
{
  
  // set up all the ParameterAttachments
  std::function<Point<float>(float)> atkToPos = [this](float value)
  {
    auto fBounds = getLocalBounds().toFloat();
    // needs to:
      // 1. figure out the max attack distance wrt fBounds
      // 2. figure out where the DragPoint should pbe placed per the attack range/skew
      
  };
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
      break;
    }
  }
}

void EnvelopeGraph::mouseDrag(const MouseEvent& e) 
{
  if(selectedPoint != nullptr)
  {

  }
}

void EnvelopeGraph::mouseUp(const MouseEvent& e) 
{

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



