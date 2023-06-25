#include "EnvelopeGraph.h"

EnvelopeGraph::EnvelopeGraph(EVT* tree, int idx) :
state(tree),
index(idx),
attackEnd(this),
holdEnd(this),
decayEnd(this),
sustainEnd(this)
{
  // set up all the ParameterAttachments
  std::function<Point<float>(float)> atkToPos = [this](float value)
  {
    
  };
  triggerAsyncUpdate();
  
}

void EnvelopeGraph::paint(Graphics& g)
{
   auto data = state->getAudioData()->getEnvelopeData(index);
   auto fBounds = getLocalBounds().toFloat();
   drawEnvelopeGraph(data, fBounds, g);
}

void EnvelopeGraph::handleAsyncUpdate()
{
  repaint();
}

//======================================================================================
void EnvelopeGraph::drawEnvelopeGraph(AHDSRData* env, Rectangle<float>& bounds, Graphics& g)
{ 
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

void EnvelopeGraph::mouseDown(const MouseEvent& e) 
{
   
}

void EnvelopeGraph::mouseDrag(const MouseEvent& e) 
{

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





