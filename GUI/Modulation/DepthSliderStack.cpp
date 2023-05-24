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
    selectButtons.add(new ModSelectButton(destID, srcID));
    addAndMakeVisible(sliders.getLast());
    addAndMakeVisible(selectButtons.getLast());
    selectedSliderIndex = sliders.size() - 1;
    resized();
}
void DepthSliderStack::removeModulationFrom(const String& srcID)
{
    static bool needToChangeSelection = false;
    //remove the slider
    for (auto s : sliders)
    {
        if (s->sourceID == srcID)
        {
            needToChangeSelection = selectedSliderIndex == s->getIndex();
            sliders.removeObject(s, true);
    
        }
    }
    for(auto b : selectButtons)
    {
        if (b->sourceID == srcID)
        {
            selectButtons.removeObject(b, true);
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
    auto lBounds = getLocalBounds().toFloat();
    // place the slider and select buttons
    for(int i = 0; i < sliders.size(); i++)
    {
        if (i == selectedSliderIndex)
        {
            sliders[i]->setVisible(true);
            sliders[i]->setBounds(lBounds.toNearestInt());
        }
        else
        {
            sliders[i]->setVisible(false);
        }
    }

}
void DepthSliderStack::paint(Graphics& g) 
{
    auto lBounds = getLocalBounds().toFloat();
    g.setColour(Color::lightGray);
    g.fillEllipse(lBounds);
    
}

void DepthSliderStack::reindexSliders()
{
    for(int idx = 0; idx < sliders.size(); ++idx)
    {
        sliders[idx]->setIndex(idx);
    }
}