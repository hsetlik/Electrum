#include "Electrum/GUI/Modulation/DestinationSlider.h"
#include "Electrum/GUI/Modulation/ModContextComponent.h"
#include "Electrum/Shared/ElectrumState.h"

DestinationSlider::DestinationSlider(ElectrumState* s, int d)
    : ModDestAttachment(d),
      state(s),
      slider(juce::Slider::Rotary, juce::Slider::TextBoxBelow),
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
  auto bounds = getLocalBounds();
  const int inset = (int)((float)bounds.getWidth() / 6.0f);
  auto sliderBounds = bounds.reduced(inset);
  slider.setBounds(sliderBounds);
  // now place the depth sliders based on that
  const int sliderHeight = sliderBounds.getHeight() - slider.getTextBoxHeight();
  const int dssWidth =
      std::min(sliderBounds.getWidth() + inset, bounds.getWidth());
  auto dssBounds = sliderBounds.removeFromTop(sliderHeight)
                       .withSizeKeepingCentre(dssWidth, dssWidth);
  depthSliders.setBounds(dssBounds);

  slider.toFront(false);
}

void DestinationSlider::paint(juce::Graphics&) {
  // we don't need to do anything here I don't think
}

void DestinationSlider::reinit() {
  depthSliders.reinitFromState();
}
