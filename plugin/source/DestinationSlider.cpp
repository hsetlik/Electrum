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
  DLog::log("Attempting to attach to param: " + id);
  // deal w the slider attachment
  attach.reset(new apvts::SliderAttachment(*state, id, slider));
}

void DestinationSlider::itemDropped(
    const juce::DragAndDropTarget::SourceDetails& dragSourceDetails) {
  const int src = dragSourceDetails.description;
  depthSliders.addModulation(src);
}

void DestinationSlider::resized() {
  auto bounds = getLocalBounds();
  // make sure our starting bounds is square
  if (bounds.getWidth() != bounds.getHeight()) {
    const int x = std::min(bounds.getWidth(), bounds.getHeight());
    bounds = bounds.withSizeKeepingCentre(x, x);
  }
  depthSliders.setBounds(bounds);
  const int inset = (int)((float)bounds.getWidth() / 6.0f);
  slider.setBounds(bounds.reduced(inset));
  slider.toFront(false);
}

void DestinationSlider::paint(juce::Graphics&) {
  // we don't need to do anything here I don't think
}

void DestinationSlider::reinit() {
  depthSliders.reinitFromState();
}
