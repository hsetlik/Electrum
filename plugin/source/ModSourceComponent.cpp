#include "Electrum/GUI/Modulation/ModSourceComponent.h"
#include "Electrum/GUI/LayoutHelpers.h"
#include "Electrum/GUI/LookAndFeel/Color.h"
#include "Electrum/Shared/ElectrumState.h"
#include "juce_core/juce_core.h"
static String _tooltipForModSource(ModSourceE id) {
  switch (id) {
    case Env1:
      return "Envelope 1";
      break;
    case Env2:
      return "Envelope 2";
      break;
    case Env3:
      return "Envelope 3";
      break;
    case ModWheel:
      return "Mod. wheel";
      break;
    case Velocity:
      return "Note Velocity";
      break;
    default:
      return "Default source";
      break;
  }
}

//===================================================

ModSourceComponent::ModSourceComponent(ElectrumState* s, int src)
    : state(s), sourceID(src) {
  setTooltip(_tooltipForModSource((ModSourceE)src));
}

void ModSourceComponent::paint(juce::Graphics& g) {
  auto fBounds = getLocalBounds().toFloat();
  fBounds = Layout::makeSquare(fBounds, true);
  g.setColour(Color::getModSourceColor((ModSourceE)sourceID));
  g.fillEllipse(fBounds);
  g.setColour(Color::black);
  g.fillEllipse(fBounds.reduced(3.0f));
}

void ModSourceComponent::mouseDown(const juce::MouseEvent& e) {
  juce::ignoreUnused(e);
  auto* dragContainer =
      juce::DragAndDropContainer::findParentDragContainerFor(this);
  if (dragContainer != nullptr) {
    dragContainer->startDragging(sourceID, this);
    return;
  }
  DLog::log("No valid DragAndDropContainer found!");
  jassert(false);
}
