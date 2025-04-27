#include "Electrum/GUI/Util/ScalableViewport.h"
#include "Electrum/Identifiers.h"
#include "juce_gui_basics/juce_gui_basics.h"
//===================================================
ScalableViewport::ScalableViewport() {
  scaleCallback = [](float nValue) {
    DBG("Normalized Scale is: " + String(nValue));
  };

  scaleSlider.setSliderStyle(juce::Slider::LinearBar);
  scaleSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 1, 1);
  scaleSlider.setRange(0.0, 1.0, 0.0001);
  scaleSlider.setValue(0.5);
  addAndMakeVisible(scaleSlider);
  scaleSlider.onValueChange = [this]() {
    const float fVal = (float)scaleSlider.getValue();
    scaleCallback(fVal);
    resized();
  };
}

void ScalableViewport::setScaleCallback(const norm_scale_callback& cb) {
  scaleCallback = cb;
}

void ScalableViewport::paint(juce::Graphics& g) {
  // check that the hbar is not out of bounds first
  auto& hBar = getHorizontalScrollBar();
  auto relBounds = hBar.getBounds();
  if (relBounds.getX() < 10) {
    resized();
  }
  this->juce::Viewport::paint(g);
}

void ScalableViewport::resized() {
  this->juce::Viewport::resized();
  auto& hBar = getHorizontalScrollBar();
  auto fBounds = hBar.getBounds().toFloat();
  const float maxScaleWidth = 150.0f;
  const float scaleWidth = std::min(maxScaleWidth, fBounds.getWidth() / 4.0f);
  auto sBounds = fBounds.removeFromLeft(scaleWidth);
  scaleSlider.setBounds(sBounds.toNearestInt());
  hBar.setBounds(fBounds.toNearestInt());
  hBar.toFront(false);
  hBar.handleUpdateNowIfNeeded();
}
