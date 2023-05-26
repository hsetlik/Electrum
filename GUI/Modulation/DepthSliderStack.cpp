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
    //fill
    auto col = Color::getColorForModSource(sourceID);
    if (!over)
    {
        col = col.darker(1.8f);
    }
    g.setColour(col);
    g.fillPath(path);
    //stroke
    PathStrokeType pst(1.5f);
    g.setColour(Color::darkSlateGray);
    g.strokePath(path, pst);

}
//====================================================================================================
ModCloseButton::ModCloseButton(const String& destID) : 
WedgeButton(destID + "CloseButton")
{

}

void ModCloseButton::paintButton(Graphics& g, bool over, bool) 
{
    auto pBounds = getParentComponent()->getLocalBounds().toFloat();
    auto fX = (float)getX();
    auto fY = (float)getY();
    auto path = getWedgePath(pBounds.getCentreX(), pBounds.getCentreY(), a1, a2, r1, r2, fX, fY);
    // fill
    auto fillColor = over ? Color::closeRedBright : Color::closeRedDark;
    g.setColour(fillColor);
    g.fillPath(path);
    //stroke
    g.setColour(Color::darkSlateGray);
    PathStrokeType pst(1.8f);
    g.strokePath(path, pst);
}
//====================================================================================================
DepthSliderStack::DepthSliderStack(EVT* tree, const String& dst) : 
state(tree), 
closeButton(dst),
destID(dst), 
selectedSliderIndex(-1)
{
    addAndMakeVisible(&closeButton);
    closeButton.onClick = [this]
    {
        this->removeSelectedSource();
    };
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
    auto selButton = selectButtons.getLast();
    selButton->onClick = [this, srcID]
    {
        this->selectSource(srcID);
    };
    addAndMakeVisible(selButton);
    selectedSliderIndex = sliders.size() - 1;
    resized();
}

void DepthSliderStack::removeModulationFrom(const String& srcID)
{
    //remove the slider
    for (auto s : sliders)
    {
        if (s->sourceID == srcID)
        {
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
    resized();
}

void DepthSliderStack::removeSelectedSource()
{
    jassert(selectedSliderIndex >= 0);
    auto srcID = selectButtons[selectedSliderIndex]->sourceID;
    removeModulationFrom(srcID);
}
    
void DepthSliderStack::selectSource(const String& sourceID)
{
    for(int i = 0; i < selectButtons.size(); i++)
    {
        if(selectButtons[i]->sourceID == sourceID)
        {
            selectedSliderIndex = i;
            resized();
            return;
        }
    }
    DLog::log("Error: source with ID " + sourceID + " not found!");
}
// component callbacks
void DepthSliderStack::resized() 
{
    //if we have no sliders, we just need to make sure the close button is hidden
    if (selectedSliderIndex == -1)
    {
        closeButton.setVisible(false);
        return;
    }
    closeButton.setVisible(true);
    //size all the sliders
    auto lBounds = getLocalBounds().toFloat();
    DLog::log("DepthSliderStack is at: " + lBounds.toString());
    auto r2 = lBounds.getWidth() / 2.0f;
    auto r1 = r2 - 10.0f;
    float baseAngle = MathConstants<float>::pi * 0.675f;
    float buttonAngle = (MathConstants<float>::pi * 0.65f) / (float)(sliders.size() + 1);
    // place the close button 
    closeButton.setStartAngle(baseAngle);
    closeButton.setEndAngle(baseAngle + buttonAngle);
    closeButton.setInnerRadius(r1);
    closeButton.setOuterRadius(r2);
    auto closeBounds = closeButton.getWedgePathForCenter(lBounds.getCentreX(), lBounds.getCentreY()).getBounds();
    closeButton.setBounds(closeBounds.toNearestInt());
    closeButton.toFront(false);
    // place the slider and select buttons
    for(int i = 0; i < sliders.size(); i++)
    {
        sliders[i]->setBounds(lBounds.toNearestInt());
        if (i == selectedSliderIndex)
        {
            sliders[i]->setVisible(true);
            sliders[i]->setEnabled(true);
            //sliders[i]->toFront(false);
            selectButtons[i]->setEnabled(false);
        }
        else
        {
            sliders[i]->setVisible(false);
            selectButtons[i]->toFront(false);
            selectButtons[i]->setEnabled(true);
        }
        // place the select button
        auto angle = baseAngle + (buttonAngle * (float)(i + 1));
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
    g.setColour(Color::lightSteelBlue);
    g.fillEllipse(lBounds);
}

void DepthSliderStack::reindexSliders()
{
    // change the selected index if we need to
    if(selectedSliderIndex >= sliders.size())
    {
        selectedSliderIndex = sliders.size() - 1;
    }
    for(int idx = 0; idx < sliders.size(); ++idx)
    {
        sliders[idx]->setIndex(idx);
        selectButtons[idx]->setIndex(idx);
    }
}