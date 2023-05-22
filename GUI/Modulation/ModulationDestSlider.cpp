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
    for (auto d : depthSliders)
    {
        d->setBounds(lBounds.toNearestInt());
    }
    paramSlider.setBounds (lBounds.reduced(lBounds.getWidth() / 4.0f).toNearestInt());
}

void ModulationDestSlider::paint(Graphics&)
{

}

void ModulationDestSlider::mouseDown(const juce::MouseEvent& e) 
{

}


void ModulationDestSlider::itemDropped(const juce::DragAndDropTarget::SourceDetails &dragSourceDetails)
{
    String srcName = dragSourceDetails.description;
    DLog::log("Dropped with description: " + srcName);
    addModulationFor(srcName);
    resized();
}

    
void ModulationDestSlider::addModulationFor(const String& srcID)
{
    depthSliders.add(new DepthSlider(state, srcID, paramID, depthSliders.size()));
    auto dSlider = depthSliders.getLast();
    addAndMakeVisible(dSlider);

}


void ModulationDestSlider::removeModulationFrom(const String& srcID)
{
    for(int i = 0; i < depthSliders.size(); i++)
    {
        if (depthSliders[i]->sourceID == srcID)
        {
            // the modulation is automatically removed from the state by the DepthSlider destructor
            depthSliders.remove(i, true);
            break;
        }
    }
    reindexDepthSliders();
    resized();
}
bool ModulationDestSlider::hasModulationFrom(const String& srcID)
{
    for(auto d : depthSliders)
    {
        if (d->sourceID == srcID)
            return true;
    }
    return false;
}

void ModulationDestSlider::reindexDepthSliders()
{
    for(int idx = 0; idx < depthSliders.size(); ++idx)
    {
        depthSliders[idx]->setIndex(idx);
    }
}