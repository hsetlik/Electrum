#include "DepthSliderStack.h"

DepthSliderStack::DepthSliderStack(EVT* tree, const String& dst) : 
state(tree), 
destID(dst), 
selectedSliderIndex(-1)
{

}

bool DepthSliderStack::hasModulationFrom(const String& srcID)
{
    for(auto s : sliders)
    {
        if (s->sourceID == srcID)
            return true;
    }
    return false;

}
void DepthSliderStack::addModulationFor(const String& srcID)
{
    sliders.add(new DepthSlider(state, srcID, destID, sliders.size()));
    addAndMakeVisible(sliders.getLast());
    selectedSliderIndex = sliders.size() - 1;
    resized();
}
void DepthSliderStack::removeModulationFrom(const String& srcID)
{
    static bool needToChangeSelection = false;
    for (auto s : sliders)
    {
        if (s->sourceID == srcID)
        {
            needToChangeSelection = selectedSliderIndex == s->getIndex();
            sliders.removeObject(s, true);
    
        }
    }
    reindexSliders();
    if (needToChangeSelection)
    {
        selectedSliderIndex = (sliders.size() > 0) ? sliders.size() - 1 : - 1;
    }
    resized();
}
// component callbacks
void DepthSliderStack::resized() 
{
    //if we have no sliders, no need to do anything
    if (selectedSliderIndex == -1)
        return;
    //size all the sliders
    auto lBounds = getLocalBounds();
    for(auto s : sliders)
    {
        s->setBounds(lBounds);
    }
    // make sure the selected slider is in front
    sliders[selectedSliderIndex]->toFront(false);
}
void DepthSliderStack::paint(Graphics& g) 
{
    auto lBounds = getLocalBounds().toFloat();
    
}

void DepthSliderStack::reindexSliders()
{
    for(int idx = 0; idx < sliders.size(); ++idx)
    {
        sliders[idx]->setIndex(idx);
    }
}