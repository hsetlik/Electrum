#include "Electrum/GUI/Modulation/DestinationSlider.h"
#include "Electrum/GUI/Modulation/ModContextComponent.h"
#include "Electrum/Shared/ElectrumState.h"

DestinationSlider::DestinationSlider(ElectrumState* s, int d)
    : ModDestAttachment(d),
      state(s),
      slider(juce::Slider::Rotary, juce::Slider::NoTextBox),
      depthSliders(s, d) {
  addAndMakeVisible(&slider);
  addAndMakeVisible(&depthSliders);
  String id = _paramIDForModDest(destID);
  // DLog::log("Attempting to attach to param: " + id);
  //  deal w the slider attachment
  attach.reset(new apvts::SliderAttachment(*state, id, slider));
}

void DestinationSlider::itemDropped(
    const juce::DragAndDropTarget::SourceDetails& dragSourceDetails) {
  const int src = dragSourceDetails.description;
  // DLog::log("Source " + String(src) + " was dropped on dest " +
  // String(destID));
  depthSliders.addModulation(src);
  repaint();
}

void DestinationSlider::resized() {
  auto fBounds = getLocalBounds().toFloat();
  // 1. make the bounds square
  auto shortSide = std::min(fBounds.getWidth(), fBounds.getHeight());
  fBounds = fBounds.withSizeKeepingCentre(shortSide, shortSide);
  // 2. place the dss
  depthSliders.setBounds(fBounds.toNearestInt());
  // 3. figure out the inset and place the main slider
  constexpr float minInset = 18.0f;
  float inset = std::max(shortSide / 5.0f, minInset);
  fBounds = fBounds.reduced(inset);
  slider.setBounds(fBounds.toNearestInt());
  slider.toFront(false);
}

void DestinationSlider::paint(juce::Graphics&) {
  // we don't need to do anything here I don't think
}

void DestinationSlider::reinit() {
  depthSliders.reinitFromState();
}
