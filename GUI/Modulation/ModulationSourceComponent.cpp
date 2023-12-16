#include "ModulationSourceComponent.h"

ModulationSourceComponent::ModulationSourceComponent(EVT *tree, const String &id)
    : state(tree), sourceID(id)
{
}

void ModulationSourceComponent::paint(Graphics &g)
{
  auto lBounds = getLocalBounds().toFloat();
  g.setColour(Color::getColorForModSource(sourceID));
  g.fillEllipse(lBounds);
  g.setColour(Color::darkBkgnd);
  g.fillEllipse(lBounds.reduced(3.0f));
}

void ModulationSourceComponent::mouseDown(const juce::MouseEvent &)
{
  DragAndDropContainer::findParentDragContainerFor(this)->startDragging(sourceID, this);
}
