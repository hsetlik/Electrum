
#include "Electrum/GUI/Modulation/ModSourceComponent.h"
#include "Electrum/GUI/LayoutHelpers.h"
#include "Electrum/GUI/LookAndFeel/Color.h"
#include "Electrum/Identifiers.h"
#include "Electrum/Shared/ElectrumState.h"
#include "juce_core/juce_core.h"
static String _tooltipForModSource(ModSourceE id) {
  switch (id) {
    case Env1:
      return "Envelope 1";
    case Env2:
      return "Envelope 2";
    case Env3:
      return "Envelope 3";
    case LFO1:
      return "LFO 1";
    case LFO2:
      return "LFO 2";
    case LFO3:
      return "LFO 3";
    case Perlin1:
      return "Perlin 1";
    case Perlin2:
      return "Perlin 2";
    case ModWheel:
      return "Mod. wheel";
    case LevelMono:
      return "Level (monophonic)";
    case LevelPoly:
      return "Level (polyphonic)";
    case Velocity:
      return "Note Velocity";
    default:
      return "Default source";
  }
}

//===================================================

ModSourceComponent::ModSourceComponent(ElectrumState* s, int src)
    : sourceID(src) {
  juce::ignoreUnused(s);
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

constexpr float _maxSourceWidth = 35.0f;
void ModSourceComponent::placeCenteredAtMaxSize(irect_t bounds) {
  auto fBounds = bounds.toFloat();
  const float width = std::min(_maxSourceWidth, fBounds.getWidth());
  const float xCenter = fBounds.getCentreX();
  const float yCenter = fBounds.getCentreY();
  frect_t squareBounds = {xCenter - (width / 2.0f), yCenter - (width / 2.0f),
                          width, width};
  setBounds(squareBounds.toNearestInt());
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
