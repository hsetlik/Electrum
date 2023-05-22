#include "DepthSlider.h"


DepthSlider::DepthSlider(EVT* tree, const String& src, const String& dst, int idx) :
state(tree),
sourceID(src),
destID(dst),
destIndex(idx),
slider(Slider::SliderStyle::Rotary, Slider::TextEntryBoxPosition::NoTextBox),
lastSliderValue(0.0f)
{
    addAndMakeVisible(&slider);
    slider.addListener(this);
    slider.setRange(-1.0f, 1.0f, 0.0001f);
    slider.setValue(0.0f);
    slider.setRotaryParameters(getDepthSliderParams());
    state->setModulation(sourceID, destID, lastSliderValue);
}

DepthSlider::~DepthSlider()
{
    state->removeModulation(sourceID, destID);
    slider.removeListener(this);
}

void DepthSlider::sliderValueChanged(Slider* s) 
{
    float newVal = (float)s->getValue();
    if (newVal != lastSliderValue)
    {
        lastSliderValue = newVal;
        state->setModulation(sourceID, destID, lastSliderValue);
    }

}

void DepthSlider::resized()
{
    slider.setBounds(getLocalBounds());
}