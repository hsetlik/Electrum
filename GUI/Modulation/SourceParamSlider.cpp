#include "SourceParamSlider.h"

SourceParamSlider::SourceParamSlider(EVT *tree, const String &ID, bool vertical)
    : state(tree), paramID(ID),
      slider((vertical) ? Slider::LinearVertical : Slider::Rotary, Slider::NoTextBox)
{
  slider.setTooltip(IDs::getParamDesc(paramID));
  addAndMakeVisible(&slider);
}

void SourceParamSlider::resized() { slider.setBounds(getLocalBounds()); }
