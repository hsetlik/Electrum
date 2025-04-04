#include "Electrum/GUI/Modulation/DestinationSlider.h"
#include "Electrum/GUI/Modulation/ModContextComponent.h"
#include "Electrum/Shared/ElectrumState.h"
#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_events/juce_events.h"

DestinationLabel::DestinationLabel(ElectrumState* s, int id)
    : state(s), destID(id) {
  addAndMakeVisible(label);
  label.setJustificationType(juce::Justification::centred);
  label.setEditable(true);
  label.addListener(this);
  // set up the parameter attachment
  auto* param = state->getParameter(_paramIDForModDest(destID));
  auto callback = [this](float value) { this->setLabelForValue(value); };
  pAttach.reset(
      new juce::ParameterAttachment(*param, callback, state->undoManager));
  pAttach->sendInitialUpdate();
}

void DestinationLabel::setLabelForValue(float val) {
  String str(std::to_string(val));
  str = str.retainCharacters("0123456789.");
  str = str.substring(0, 5);
  label.setText(str, juce::dontSendNotification);
}
void DestinationLabel::labelTextChanged(juce::Label* l) {
  auto str = l->getText().retainCharacters("0123456789.");
  const float fVal = std::stof(str.toStdString());
  if (!fequal(fVal, currentParamVal)) {
    currentParamVal = fVal;
    pAttach->setValueAsCompleteGesture(fVal);
  }
}
//=============================================================
DestinationSlider::DestinationSlider(ElectrumState* s, int d)
    : ModDestAttachment(d),
      state(s),
      slider(juce::Slider::Rotary, juce::Slider::NoTextBox),
      label(s, d),
      depthSliders(s, d) {
  addAndMakeVisible(&slider);
  addAndMakeVisible(&depthSliders);
  addAndMakeVisible(&label);
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
  // 1. grab an area for the label
  static const float minLabelHeight = 24.0f;
  auto lBounds = fBounds.removeFromBottom(minLabelHeight);
  label.setBounds(lBounds.toNearestInt());
  // 2. make the bounds square
  auto shortSide = std::min(fBounds.getWidth(), fBounds.getHeight());
  fBounds = fBounds.withSizeKeepingCentre(shortSide, shortSide);
  // 3. place the dss
  depthSliders.setBounds(fBounds.toNearestInt());
  // 4. figure out the inset and place the main slider
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
