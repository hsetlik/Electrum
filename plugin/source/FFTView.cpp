#include "Electrum/GUI/WaveEditor/FFTView.h"
#include "Electrum/Audio/AudioUtil.h"
#include "Electrum/Audio/Wavetable.h"
#include "Electrum/GUI/LookAndFeel/Color.h"
#include "Electrum/Identifiers.h"
#include "juce_core/juce_core.h"

static float getMagnitudeAtNormFreq(const bin_array_t& bins, float x) {
  const float fBin = x * (float)(AUDIBLE_BINS - 1);
  size_t bLow = AudioUtil::fastFloor64(fBin);
  size_t bHigh = bLow + 1;
  const float t = fBin - (float)bLow;
  return flerp(bins[bLow].magnitude, bins[bHigh].magnitude, t);
}

//===================================================
FrameSpectrum::FrameSpectrum(ValueTree& vt) : WaveEditListener(vt) {
  frameWasFocused(0);
}

void FrameSpectrum::frameWasFocused(int frame) {
  if (frame != currentFrame) {
    binsReady = false;
    currentFrame = frame;
    auto child = waveTree.getChild(frame);
    jassert(child.isValid() && child.hasType(WaveEdit::WAVE_FRAME));
    String waveStr = child[WaveEdit::frameStringData];
    currentMaxMagnitude = Wave::loadAudibleBins(waveStr, loadedWaveBins, false);
    // now find the median
    float median = Wave::getMedianBinMagnitude(loadedWaveBins);
    // and set up the y range
    currentMagRange = rangeWithCenter(0.0f, currentMaxMagnitude, median);
    binsReady = true;
    repaint();
  }
}

float FrameSpectrum::yPosForFreq(const frect_t& bounds, float fNorm) const {
  if (!binsReady)
    return bounds.getBottom();
  float magnitude = getMagnitudeAtNormFreq(loadedWaveBins, fNorm);
  if (!currentMagRange.getRange().contains(magnitude)) {
    magnitude = currentMagRange.snapToLegalValue(magnitude);
  }
  const float yNorm = currentMagRange.convertTo0to1(magnitude);
  return bounds.getBottom() - (yNorm * bounds.getHeight());
}

void FrameSpectrum::paint(juce::Graphics& g) {
  auto fBounds = getLocalBounds().toFloat();
  g.setColour(UIColor::windowBkgnd);
  g.fillRect(fBounds);
  juce::Path path;
  path.startNewSubPath(fBounds.getX(), fBounds.getBottom());
  const float dX = fBounds.getWidth() / (float)FFT_GRAPH_RES;
  float x = fBounds.getX();
  float y;
  for (size_t i = 1; i < FFT_GRAPH_RES; ++i) {
    const float xNorm = (float)i / (float)FFT_GRAPH_RES;
    y = yPosForFreq(fBounds, xNorm);
    path.lineTo(x, y);
    x += dX;
  }
  path.lineTo(fBounds.getWidth(), fBounds.getHeight());
  path.closeSubPath();
  g.setColour(Color::literalOrangeBright);
  g.fillPath(path);
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

//===================================================

FrameSpectrumViewer::FrameSpectrumViewer(ValueTree& vt) : spec(vt) {
  // 1. set up the view port
  vpt.setViewedComponent(&spec, false);
  vpt.setViewPosition(0, 0);
  vpt.setInterceptsMouseClicks(true, true);
  addAndMakeVisible(vpt);
  spec.resized();
  // 2. set up the zoom slider
  slider.setSliderStyle(juce::Slider::LinearVertical);
  slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 1, 1);
  slider.setRange(0.0, 1.0);
  addAndMakeVisible(slider);
  slider.onValueChange = [this]() {
    const float zNorm = (float)slider.getValue();
    spec.setZoomNorm(zNorm);
  };
}

void FrameSpectrumViewer::resized() {
  auto iBounds = getLocalBounds();
  auto sBounds = iBounds.removeFromLeft(30);
  slider.setBounds(sBounds);
  vpt.setBounds(iBounds);
}
void FrameSpectrumViewer::paint(juce::Graphics& g) {
  auto fBounds = getLocalBounds().toFloat();
  g.setColour(UIColor::menuBkgnd);
  g.fillRect(fBounds);
}
