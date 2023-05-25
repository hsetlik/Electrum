#include "DepthSliderStack.h"

ModSelectButton::ModSelectButton(OwnedArray<ModSelectButton>* arr, const String& srcName) :
WedgeButton(srcName + "SelectButton"),
allButtons(arr),
sourceID(srcName)
{

}

void ModSelectButton::paintButton(Graphics& g, bool , bool )
{
    auto path = getCurrentWedgePath();
    g.setColour(Color::darkGray);
    g.fillPath(path);

    
}
//====================================================================================================
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
    selectButtons.add(new ModSelectButton(&selectButtons, srcID));
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
        if(b->sourceID == srcID)
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
    DLog::log("DepthSliderStack is at: " + lBounds.toString());
    auto r2 = lBounds.getWidth() / 2.0f;
    auto r1 = r2 - 10.0f;
    const float baseAngle = MathConstants<float>::pi * 1.25f;
    float buttonAngle = MathConstants<float>::pi / (float)(sliders.size() + 1);
    // place the slider and select buttons
    for(int i = 0; i < sliders.size(); i++)
    {
        // place the select button
        auto angle = baseAngle + (buttonAngle * (float)(i + 1));
        selectButtons[i]->setWedgeParams(angle, angle + buttonAngle, r1, r2);
        selectButtons[i]->resized();
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