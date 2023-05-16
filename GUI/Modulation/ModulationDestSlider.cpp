#include "ModulationDestSlider.h"

ModulationDestSlider::ModulationDestSlider(EVT* tree, const String& id) :
state(tree),
paramID(id)
{
    paramSlider.setSliderStyle(Slider::SliderStyle::Rotary);
    paramSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 1, 1);
    addAndMakeVisible(&paramSlider);
    //attach.reset(new sAttach(*state, paramID, paramSlider));
    DLog::log("Added slider with ID: " + paramID);
}


void ModulationDestSlider::resized()
{
    auto lBounds = getLocalBounds().toFloat();
    paramSlider.setBounds (lBounds.reduced(lBounds.getWidth() / 3.0f).toNearestInt());
}

void ModulationDestSlider::paint(Graphics& g)
{
    g.fillAll(Colour::fromRGB(82, 222, 196));
}