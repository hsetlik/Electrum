#include "ModulationDestSlider.h"

ModulationDestSlider::ModulationDestSlider(EVT* tree, const String& id) :
state(tree),
paramID(id),
stack(tree, id)
{
    addAndMakeVisible(&stack);
    attach.reset(new sAttach(*state->getAPVTS(), paramID, paramSlider));
    paramSlider.setSliderStyle(Slider::SliderStyle::Rotary);
    paramSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, false, 0, 0);
    addAndMakeVisible(&paramSlider);
}


void ModulationDestSlider::resized()
{
    auto lBounds = getLocalBounds().toFloat();
    float shorter = std::min(lBounds.getWidth(), lBounds.getHeight());
    lBounds = Rectangle<float>(0.0f, 0.0f, shorter, shorter);
    stack.setBounds(lBounds.toNearestInt());
    paramSlider.setBounds (lBounds.reduced(lBounds.getWidth() / 4.0f).toNearestInt());
    paramSlider.toFront(false);
}

void ModulationDestSlider::paint(Graphics&)
{

}

void ModulationDestSlider::itemDropped(const juce::DragAndDropTarget::SourceDetails &dragSourceDetails)
{
    String srcName = dragSourceDetails.description;
    addModulationFor(srcName);
    repaint();
    DLog::log("Dropped with description: " + srcName);
}

    
void ModulationDestSlider::addModulationFor(const String& srcID)
{
    stack.addModulationFor(srcID);
}


void ModulationDestSlider::removeModulationFrom(const String& srcID)
{
    stack.removeModulationFrom(srcID);
}
bool ModulationDestSlider::hasModulationFrom(const String& srcID)
{
    return stack.hasModulationFrom(srcID);
}

