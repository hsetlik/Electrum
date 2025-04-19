#include "Electrum/GUI/WaveEditor/FFTView.h"
#include "Electrum/Audio/AudioUtil.h"
#include "Electrum/Audio/Wavetable.h"
#include "Electrum/GUI/LookAndFeel/Color.h"
#include "Electrum/GUI/LookAndFeel/Fonts.h"
#include "Electrum/GUI/WaveEditor/EditValueTree.h"
#include "Electrum/GUI/WaveEditor/WaveEdiorContext.h"
#include "Electrum/Identifiers.h"
#include "juce_core/juce_core.h"

// Mouse handling=============================================
static float s_yToNormMag(const frect_t& bounds, float yPos) {
  return (bounds.getBottom() - yPos) / bounds.getHeight();
}
void FrameSpectrum::mouseDown(const juce::MouseEvent& m) {
  auto fBounds = getLocalBounds().toFloat();
  selectedPt = warp->editablePointNear(fBounds, m.position);
}

void FrameSpectrum::mouseUp(const juce::MouseEvent& m) {
  if (selectedPt != nullptr) {
    auto fBounds = getLocalBounds().toFloat();
    const float nFreq = m.position.x / fBounds.getWidth();
    const float nMag = s_yToNormMag(fBounds, m.position.y);
    warp->placePoint(selectedPt, nMag, nFreq);
    refreshNeeded = true;
    waveTreeNeedsUpdate = true;
    selectedPt = nullptr;
  }
}

void FrameSpectrum::mouseDrag(const juce::MouseEvent& m) {
  if (selectedPt != nullptr) {
    auto fBounds = getLocalBounds().toFloat();
    const float nFreq = m.position.x / fBounds.getWidth();
    const float nMag = s_yToNormMag(fBounds, m.position.y);
    warp->placePoint(selectedPt, nMag, nFreq);
    refreshNeeded = true;
    waveTreeNeedsUpdate = true;
  }
}

void FrameSpectrum::mouseDoubleClick(const juce::MouseEvent& m) {
  auto fBounds = getLocalBounds().toFloat();
  const float nFreq = m.position.x / fBounds.getWidth();
  const float nMag = s_yToNormMag(fBounds, m.position.y);
  auto* existing = warp->editablePointNear(fBounds, m.position);
  if (existing != nullptr) {
    warp->deletePoint(existing);
    refreshNeeded = true;
    waveTreeNeedsUpdate = true;
  } else if (warp->isNearEditLine(nMag, nFreq)) {
    warp->createWarpPoint(nMag, nFreq);
    refreshNeeded = true;
    waveTreeNeedsUpdate = true;
  }
}

//===================================================

FrameSpectrum::FrameSpectrum(ValueTree& vt)
    : WaveEditListener(vt), warp(nullptr) {
  setRepaintsOnMouseActivity(false);
  frameWasFocused(0);
  startTimerHz(SPECTRUM_REFRESH_HZ);
}

void FrameSpectrum::paint(juce::Graphics& g) {
  auto fBounds = getLocalBounds().toFloat();
  auto* vpt = findParentComponentOfClass<juce::Viewport>();
  jassert(vpt != nullptr);
  if (currentFrame > -1) {
    auto viewBounds = vpt->getViewArea().toFloat();
    const float startNorm = viewBounds.getX() / fBounds.getWidth();
    const float endNorm = viewBounds.getRight() / fBounds.getWidth();
    warp->drawSpectrumRange(g, viewBounds, startNorm, endNorm, selectedPt);
  }
}

void FrameSpectrum::timerCallback() {
  if (refreshNeeded) {
    repaint();
    refreshNeeded = false;
  }
  auto currentMs = juce::Time::getApproximateMillisecondCounter();
  if ((currentMs - lastOscUpdateMs) > oscUpdateInterval && isVisible()) {
    auto* context = findParentComponentOfClass<WaveEditorContext>();
    if (context != nullptr) {
      context->previewEditsOnOscillator();
    }
  }
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
  if (currentFrame != frame && frame != -1) {
    currentFrame = frame;
    auto frameTree = waveTree.getChild(currentFrame);
    warp.reset(new FrameWarp(frameTree));
    resized();
    refreshNeeded = true;
  }
}

void FrameSpectrum::waveTreeUpdateRequested() {
  if (waveTreeNeedsUpdate) {
    auto frame = waveTree.getChild(currentFrame);
    auto oldWarpTree = waveTree.getChildWithName(WaveEdit::FFT_WARP);
    if (oldWarpTree.isValid()) {
      frame.removeChild(oldWarpTree, nullptr);
    }
    auto newChild = warp->getWarpTree(true);
    frame.appendChild(newChild, nullptr);
    waveTreeNeedsUpdate = false;
  }
}

//===================================================

FrameSpectrumViewer::FrameSpectrumViewer(ValueTree& vt) : spec(vt) {
  // 1. set up the view port
  vpt.setViewedComponent(&spec, false);
  vpt.setViewPosition(0, 0);
  vpt.setInterceptsMouseClicks(true, true);
  vpt.setRepaintsOnMouseActivity(true);
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
