#include "ModulationDestSlider.h"

ModulationDestSlider::ModulationDestSlider(EVT* tree, const String& id) :
state(tree),
paramID(id)
{
    attach.reset(new sAttach(*state->getAPVTS(), paramID, paramSlider));
    paramSlider.setSliderStyle(Slider::SliderStyle::Rotary);
    paramSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, false, 0, 0);
    addAndMakeVisible(&paramSlider);
}


void ModulationDestSlider::resized()
{
    auto lBounds = getLocalBounds().toFloat();
    paramSlider.setBounds (lBounds.reduced(lBounds.getWidth() / 4.0f).toNearestInt());
}

void ModulationDestSlider::paint(Graphics& g)
{
    g.fillAll(Colour::fromRGB(82, 222, 196));
    g.setColour(Colour::fromRGB(252, 161, 3));
    g.fillRect(paramSlider.getBounds().toFloat());

}