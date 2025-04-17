#include "Electrum/GUI/WaveEditor/FFTView.h"
#include "Electrum/Audio/AudioUtil.h"
#include "Electrum/Audio/Wavetable.h"
#include "Electrum/GUI/LookAndFeel/Color.h"
#include "Electrum/GUI/LookAndFeel/Fonts.h"
#include "Electrum/GUI/WaveEditor/EditValueTree.h"
#include "Electrum/Identifiers.h"
#include "juce_core/juce_core.h"

// Mouse handling=============================================
void FrameSpectrum::mouseDown(const juce::MouseEvent& m) {}

void FrameSpectrum::mouseUp(const juce::MouseEvent&) {}

void FrameSpectrum::mouseDrag(const juce::MouseEvent& m) {}

void FrameSpectrum::mouseDoubleClick(const juce::MouseEvent& m) {}

//===================================================
static void s_drawEditHandle(juce::Graphics& g, fpoint_t center) {
  frect_t rect;
  rect.setCentre(center);
  rect = rect.withSizeKeepingCentre(12.0f, 12.0f);
  g.fillEllipse(rect);
}

FrameSpectrum::FrameSpectrum(ValueTree& vt) : WaveEditListener(vt) {}

void FrameSpectrum::paint(juce::Graphics& g) {
  // 1. fill the background
  auto fBounds = getLocalBounds().toFloat();
  g.setColour(UIColor::windowBkgnd);
  g.fillRect(fBounds);
  auto* vpt = findParentComponentOfClass<juce::Viewport>();
}

void FrameSpectrum::setZoomNorm(float v) {
  currentZoom = flerp(MIN_FFT_ZOOM, MAX_FFT_ZOOM, v);
  resized();
}

void FrameSpectrum::resized() {
  int height = std::max(getLocalBounds().getHeight(), 250);
  int width = (int)(currentZoom * (float)FFT_GRAPH_RES);
  auto* parent = findParentComponentOfClass<juce::Viewport>();
  if (parent != nullptr) {
    height = parent->getMaximumVisibleHeight();
    const int visWidth = parent->getMaximumVisibleWidth();
    // ensure we can't zoom in so far that
    // part of the viewport is empty
    if (width < visWidth) {
      width = visWidth;
      currentZoom = (float)width / (float)FFT_GRAPH_RES;
    }
  }
  setSize(width, height);
}

void FrameSpectrum::frameWasFocused(int frame) {
  if (currentFrame != frame) {
    currentFrame = frame;
  }
}

//===================================================

FrameSpectrumViewer::FrameSpectrumViewer(ValueTree& vt) : spec(vt) {
  // 1. set up the view port
  vpt.setViewedComponent(&spec, false);
  vpt.setViewPosition(0, 0);
  vpt.setInterceptsMouseClicks(true, true);
  addAndMakeVisible(vpt);
  spec.resized();
  // 2. set up the zoom slider
  zoomSlider.setSliderStyle(juce::Slider::LinearVertical);
  zoomSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 1, 1);
  zoomSlider.setRange(0.0, 1.0);
  addAndMakeVisible(zoomSlider);
  zoomSlider.onValueChange = [this]() {
    const float zNorm = (float)zoomSlider.getValue();
    spec.setZoomNorm(zNorm);
  };

  // 4. set up the label strings
  zoomStr.aString.setText("Zoom");
  zoomStr.aString.setJustification(juce::Justification::centred);
  zoomStr.aString.setFont(FontData::getFontWithHeight(FontE::RobotoMI, 12.0f));
  zoomStr.aString.setColour(UIColor::defaultText);

  zoomSlider.setValue(0.3f);
}

void FrameSpectrumViewer::resized() {
  auto fBounds = getLocalBounds().toFloat();
  auto controlWidth = std::max(fBounds.getWidth() / 10.0f, 45.0f);
  auto zoomBounds = fBounds.removeFromLeft(controlWidth);
  zoomStr.bounds = zoomBounds.removeFromTop(14.0f);
  zoomSlider.setBounds(zoomBounds.toNearestInt());
  vpt.setBounds(fBounds.toNearestInt());
}

void FrameSpectrumViewer::paint(juce::Graphics& g) {
  auto fBounds = getLocalBounds().toFloat();
  g.setColour(UIColor::menuBkgnd);
  g.fillRect(fBounds);
  zoomStr.draw(g);
}
