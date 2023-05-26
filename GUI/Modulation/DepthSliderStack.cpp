#include "DepthSliderStack.h"

ModSelectButton::ModSelectButton(int& sel, int idx, const String& srcName) :
WedgeButton(srcName + "SelectButton"),
selectedIdx(&sel),
index(idx),
sourceID(srcName)
{

}

void ModSelectButton::paintButton(Graphics& g, bool over , bool )
{
    auto pBounds = getParentComponent()->getLocalBounds().toFloat();
    auto fX = (float)getX();
    auto fY = (float)getY();
    auto path = getWedgePath(pBounds.getCentreX(), pBounds.getCentreY(), a1, a2, r1, r2, fX, fY);
    auto col = over ? Color::closeRedBright : Color::closeRedDark;
    g.setColour(col);
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
    selectButtons.add(new ModSelectButton(selectedSliderIndex, selectButtons.size(), srcID));
    addAndMakeVisible(sliders.getLast());
    addAndMakeVisible(selectButtons.getLast());
    selectedSliderIndex = sliders.size() - 1;
    resized();
}
void DepthSliderStack::removeModulationFrom(const String& srcID)
{
    static bool needToChangeSelection = (selectedSliderIndex == -1);
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
    const float baseAngle = MathConstants<float>::pi * 0.75f;
    float buttonAngle = (MathConstants<float>::pi * 0.65f) / (float)(sliders.size() + 2);
    // place the slider and select buttons
    for(int i = 0; i < sliders.size(); i++)
    {

        if (i == selectedSliderIndex)
        {
            sliders[i]->setVisible(true);
            sliders[i]->setEnabled(true);
            sliders[i]->setBounds(lBounds.toNearestInt());
            //sliders[i]->toFront(false);
        }
        else
        {
            sliders[i]->setVisible(false);
        }
        // place the select button
        auto angle = baseAngle + (buttonAngle * (float)i);
        selectButtons[i]->setStartAngle(angle);
        selectButtons[i]->setEndAngle(angle + buttonAngle);
        selectButtons[i]->setInnerRadius(r1);
        selectButtons[i]->setOuterRadius(r2);
        auto bBounds = selectButtons[i]->getWedgePathForCenter(lBounds.getCentreX(), lBounds.getCentreY()).getBounds();
        selectButtons[i]->setBounds(bBounds.toNearestInt());

    }

}
void DepthSliderStack::paint(Graphics& g) 
{
    auto lBounds = getLocalBounds().toFloat();
    g.setColour(Color::lightGray);
    g.fillEllipse(lBounds);

    for(auto b : selectButtons)
    {
        g.setColour(Color::lightTeal);
        auto buttonPath = b->getWedgePathForCenter(lBounds.getCentreX(), lBounds.getCentreY());
        DLog::log("Path in parent: " + buttonPath.toString().substring(0, 10));
        g.fillPath(buttonPath);
    }



}

void DepthSliderStack::reindexSliders()
{
    for(int idx = 0; idx < sliders.size(); ++idx)
    {
        sliders[idx]->setIndex(idx);
    }
}