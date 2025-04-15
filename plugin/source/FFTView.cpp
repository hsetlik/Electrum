#include "Electrum/GUI/WaveEditor/FFTView.h"
#include "Electrum/Audio/AudioUtil.h"
#include "Electrum/Audio/Wavetable.h"
#include "Electrum/GUI/LookAndFeel/Color.h"
#include "Electrum/GUI/LookAndFeel/Fonts.h"
#include "Electrum/GUI/WaveEditor/EditValueTree.h"
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
// helpers for

FrameWarp::FrameWarp(FrameSpectrum* s) : parent(s) {
  auto child = parent->waveTree.getChild(parent->currentFrame);
  jassert(child.isValid() && child.hasType(WaveEdit::WAVE_FRAME));
  auto warpTree = child.getChildWithName(WaveEdit::FFT_WARP);
  if (warpTree.isValid()) {  // we already have an active frame warp
    for (auto it = warpTree.begin(); it != warpTree.end(); ++it) {
      auto gainPt = *it;
      jassert(gainPt.hasType(WaveEdit::FFT_GAIN_POINT));
      const float freq = gainPt[WaveEdit::gainPointFrequency];

      const float mag = gainPt[WaveEdit::gainPointMagnitude];
      auto* wp = touchedPoints.add(new WarpPoint(freq, mag));
      editPoints.push_back(wp);
    }
  }
  sortEditPoints();
}

static std::function<bool(WarpPoint* a, WarpPoint* b)> wpCompare =
    [](WarpPoint* a, WarpPoint* b) { return a->normFreq < b->normFreq; };

void FrameWarp::sortEditPoints() {
  std::sort(editPoints.begin(), editPoints.end(), wpCompare);
}

//===================================================
FrameSpectrum::FrameSpectrum(ValueTree& vt)
    : WaveEditListener(vt), warp(std::make_unique<FrameWarp>(this)) {
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

void FrameSpectrum::setNumWarpPoints(int points) {
  juce::ignoreUnused(points);
}

float FrameSpectrum::yPosAtFreq(const frect_t& bounds, float fNorm) const {
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
    y = yPosAtFreq(fBounds, xNorm);
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
  zoomSlider.setSliderStyle(juce::Slider::LinearVertical);
  zoomSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 1, 1);
  zoomSlider.setRange(0.0, 1.0);
  addAndMakeVisible(zoomSlider);
  zoomSlider.onValueChange = [this]() {
    const float zNorm = (float)zoomSlider.getValue();
    spec.setZoomNorm(zNorm);
  };
  // 3. set up the point slider
  pointSlider.setSliderStyle(juce::Slider::LinearVertical);
  pointSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 35, 18);
  pointSlider.setRange((double)MIN_WARP_PTS, (double)MAX_WARP_PTS, 1.0);
  pointSlider.setValue((double)DEFAULT_WARP_POINTS);
  addAndMakeVisible(pointSlider);
  if (spec.numWarpPoints() != -1) {
    pointSlider.setValue((double)spec.numWarpPoints());
  } else {
    pointSlider.setVisible(false);
    pointSlider.setEnabled(false);
  }
  pointSlider.onValueChange = [this]() {
    int nPoints = (int)pointSlider.getValue();
    spec.setNumWarpPoints(nPoints);
  };
  // 4. set up the label strings
  zoomStr.aString.setText("Zoom");
  zoomStr.aString.setJustification(juce::Justification::centred);
  zoomStr.aString.setFont(FontData::getFontWithHeight(FontE::RobotoMI, 12.0f));
  zoomStr.aString.setColour(UIColor::defaultText);

  pointStr.aString.setText("Warp Points");
  pointStr.aString.setJustification(juce::Justification::centred);
  pointStr.aString.setFont(FontData::getFontWithHeight(FontE::RobotoMI, 12.0f));
  pointStr.aString.setColour(UIColor::defaultText);

  zoomSlider.setValue(0.0003);
}

void FrameSpectrumViewer::resized() {
  if (spec.numWarpPoints() != -1) {
    pointSlider.setVisible(true);
    pointSlider.setEnabled(true);
  } else {
    pointSlider.setVisible(false);
    pointSlider.setEnabled(false);
  }
  auto fBounds = getLocalBounds().toFloat();
  auto controlWidth = std::max(fBounds.getWidth() / 10.0f, 45.0f);
  auto zoomBounds = fBounds.removeFromLeft(controlWidth);
  zoomStr.bounds = zoomBounds.removeFromTop(14.0f);
  auto pointBounds = zoomBounds.removeFromBottom(zoomBounds.getHeight() / 2.0f);
  pointStr.bounds = pointBounds.removeFromTop(14.0f);
  zoomSlider.setBounds(zoomBounds.toNearestInt());
  pointSlider.setBounds(pointBounds.toNearestInt());
  vpt.setBounds(fBounds.toNearestInt());
}

void FrameSpectrumViewer::paint(juce::Graphics& g) {
  auto fBounds = getLocalBounds().toFloat();
  g.setColour(UIColor::menuBkgnd);
  g.fillRect(fBounds);
  zoomStr.draw(g);
  if (pointSlider.isVisible()) {
    pointStr.draw(g);
  }
}
