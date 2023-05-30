#include "EnvelopePanel.h"
EnvelopePanel::EnvelopePanel(EVT* tree, int idx) :
state(tree),
index(idx)
{

}


void EnvelopePanel::paint(Graphics& g)
{
    auto* envData = state->getAudioData()->getEnvelopeData(index);
    auto lBounds = getLocalBounds().toFloat();
    
}



void EnvelopePanel::mouseDown(const MouseEvent &event) 
{

}
void EnvelopePanel::mouseDrag(const MouseEvent &event)
{

}
void EnvelopePanel::mouseUp(const MouseEvent &event)
{

}