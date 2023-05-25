#include "ModulationSourceComponent.h"

ModulationSourceComponent::ModulationSourceComponent(EVT* tree, const String& id) :
state (tree),
sourceID(id)
{

}

void ModulationSourceComponent::paint(Graphics& g) 
{
    auto lBounds = getLocalBounds().toFloat();
    g.setColour(Colour::fromRGB(18, 230, 212));
    g.fillEllipse(lBounds);
    g.setColour(Colour::fromRGB(117, 117, 117));
    g.fillEllipse(lBounds.reduced(3.0f));
}

    
void ModulationSourceComponent::mouseDown(const juce::MouseEvent&) 
{
    DragAndDropContainer::findParentDragContainerFor(this)->startDragging(sourceID, this);
}