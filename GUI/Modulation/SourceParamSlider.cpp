#include "SourceParamSlider.h"

SourceParamSlider::SourceParamSlider(EVT* tree, const String& ID) :
state(tree),
paramID(ID),
slider(Slider::Rotary, Slider::NoTextBox)
{
    attach.reset(new sAttach(*state->getAPVTS(), paramID, slider));
    addAndMakeVisible(&slider);
}

void SourceParamSlider::resized()
{
    slider.setBounds(getLocalBounds());
}