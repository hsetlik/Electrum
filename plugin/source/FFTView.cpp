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

static frange_t freqToXRange =
    rangeWithCenter(0.0f, 1.0f, 250.0f / (float)AUDIBLE_BINS);

static float normFreqToXPos(const frect_t& bounds, float freq) {
  float n = freqToXRange.convertTo0to1(freq);
  return bounds.getX() + (n * bounds.getWidth());
}

static float xToNormFreq(const frect_t& bounds, float xPos) {
  float n = (xPos - bounds.getX()) / bounds.getWidth();
  return freqToXRange.convertFrom0to1(n);
}

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

static WarpPoint* findLeftNeighbor(std::vector<WarpPoint*>& points,
                                   float fNorm) {
  jassert(points.size() > 2);

  for (size_t r = 1; r < points.size(); ++r) {
    auto* rPt = points[r];
    auto* lPt = points[r - 1];
    if (lPt->normFreq <= fNorm && rPt->normFreq > fNorm) {
      return lPt;
    }
  }
  jassert(false);
  return nullptr;
}

static WarpPoint* findRightNeighbor(std::vector<WarpPoint*>& points,
                                    float fNorm) {
  jassert(points.size() > 2);

  for (size_t r = 1; r < points.size(); ++r) {
    auto* rPt = points[r];
    auto* lPt = points[r - 1];
    if (lPt->normFreq <= fNorm && rPt->normFreq > fNorm) {
      return rPt;
    }
  }
  jassert(false);
  return nullptr;
}

void FrameWarp::updateWarpBins(const bin_array_t& loaded, bin_array_t& warped) {
  // copy the original values over
  for (size_t i = 0; i < AUDIBLE_BINS; ++i) {
    warped[i] = loaded[i];
  }
  if (editPoints.size() > 0) {  //  then apply the gain for each bin
    float fNorm, t;
    for (size_t i = 0; i < AUDIBLE_BINS; ++i) {
      fNorm = (float)i / (float)AUDIBLE_BINS;
      auto* left = findLeftNeighbor(editPoints, fNorm);
      auto* right = findRightNeighbor(editPoints, fNorm);
      t = (fNorm - left->normFreq) / (right->normFreq - left->normFreq);
      float lGain = left->warpedMag / left->originalMag;
      float rGain = left->warpedMag / left->originalMag;
      warped[i].magnitude *= flerp(lGain, rGain, t);
    }
  }
}

WarpPoint* FrameWarp::getPointNear(const frect_t& bounds,
                                   const fpoint_t& point) {
  if (editPoints.size() < 2)
    return nullptr;
  // NOTE: we don't check the first and last edit points
  // because they aren't allowed to be moved
  for (size_t i = 1; i < editPoints.size() - 1; ++i) {
    auto pt = parent->warpPointToCanvas(bounds, editPoints[i]);
    if (point.getDistanceFrom(pt) < MAX_CLICK_DISTANCE) {
      return editPoints[i];
    }
  }
  return nullptr;
}

bool FrameWarp::canMoveTo(WarpPoint* pt, float freq) {
  auto* left = findLeftNeighbor(editPoints, pt->normFreq);
  auto* right = findRightNeighbor(editPoints, pt->normFreq);
  return (freq > left->normFreq) && (freq < right->normFreq);
}

void FrameWarp::createWarpPoint(float nFreq, float mag) {
  // if this is the first point, we need to create the fixed points
  // at the start and end before creating this one
  if (touchedPoints.size() < 1) {
    const float magStart = parent->loadedWaveBins[0].magnitude;
    const float magEnd = parent->loadedWaveBins[AUDIBLE_BINS - 1].magnitude;
    auto* pStart = touchedPoints.add(new WarpPoint(0.0f, magStart));
    auto* pEnd = touchedPoints.add(new WarpPoint(0.999999999f, magEnd));
    editPoints.push_back(pStart);
    editPoints.push_back(pEnd);
    parent->warpPoints = 2;
  }
  auto* newPt = touchedPoints.add(new WarpPoint(nFreq, mag));
  editPoints.push_back(newPt);
  sortEditPoints();
  ++parent->warpPoints;
}
void FrameWarp::removeWarpPoint(WarpPoint* pt) {
  // 1. remove it from the editPoints array
  for (auto it = editPoints.begin(); it != editPoints.end(); ++it) {
    if (*it == pt) {
      editPoints.erase(it);
      it = editPoints.end();
    }
  }
  // 2. delete it from the touchedPoints array
  touchedPoints.removeObject(pt);
  --parent->warpPoints;
  // 3. if we removed the last user-editable point,
  // clear both arrays
  if (touchedPoints.size() <= 2) {
    editPoints.clear();
    touchedPoints.clear();
  } else {
    sortEditPoints();
  }
}

// Mouse handling=============================================
void FrameSpectrum::mouseDown(const juce::MouseEvent& m) {
  auto fBounds = getLocalBounds().toFloat();
  selectedPoint = warp->getPointNear(fBounds, m.position);
}

void FrameSpectrum::mouseUp(const juce::MouseEvent&) {
  if (selectedPoint != nullptr) {
    warpUpdateNeeded = true;
    selectedPoint = nullptr;
  }
}

void FrameSpectrum::mouseDrag(const juce::MouseEvent& m) {
  if (selectedPoint != nullptr) {
    auto fBounds = getLocalBounds().toFloat();
    const float fNorm = xToNormFreq(fBounds, m.position.x);
    const float mag = magForYPos(fBounds, m.position.y);
    if (warp->canMoveTo(selectedPoint, fNorm)) {
      selectedPoint->warpedMag = mag;
      selectedPoint->normFreq = fNorm;
      warpUpdateNeeded = true;
    }
  }
}

void FrameSpectrum::mouseDoubleClick(const juce::MouseEvent& m) {
  auto fBounds = getLocalBounds().toFloat();
  auto* existingPoint = warp->getPointNear(fBounds, m.position);
  if (existingPoint != nullptr) {
    warp->removeWarpPoint(existingPoint);
    warpUpdateNeeded = true;
  } else if (isNearEditLine(fBounds, m.position)) {
    const float nFreq = xToNormFreq(fBounds, m.position.x);
    const float mag = magForYPos(fBounds, m.position.y);
    warp->createWarpPoint(nFreq, mag);
    warpUpdateNeeded = true;
  }
}

//===================================================

FrameSpectrum::FrameSpectrum(ValueTree& vt)
    : WaveEditListener(vt), warp(nullptr) {
  frameWasFocused(0);
  setRepaintsOnMouseActivity(true);
  startTimerHz(WAVE_UPDATE_HZ);
}

void FrameSpectrum::timerCallback() {
  if (warpUpdateNeeded) {
    warp->updateWarpBins(loadedWaveBins, warpWaveBins);
    warpUpdateNeeded = false;
  }
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
    // and reinstantiate the FrameWarp object for the new frame
    warp.reset(new FrameWarp(this));
    int numPts = warp->numEditPoints();
    warpPoints = (numPts > 0) ? numPts : -1;
    binsReady = true;
    repaint();
  }
}

void FrameSpectrum::setNumWarpPoints(int points) {
  juce::ignoreUnused(points);
}

float FrameSpectrum::yPosLoaded(const frect_t& bounds, float fNorm) const {
  if (!binsReady)
    return bounds.getBottom();
  float magnitude = getMagnitudeAtNormFreq(loadedWaveBins, fNorm);
  return yPosForMag(bounds, magnitude);
}

float FrameSpectrum::yPosWarped(const frect_t& bounds, float fNorm) const {
  if (!binsReady)
    return bounds.getBottom();
  float magnitude = getMagnitudeAtNormFreq(warpWaveBins, fNorm);
  return yPosForMag(bounds, magnitude);
}

float FrameSpectrum::yPosForMag(const frect_t& bounds, float magnitude) const {
  if (!currentMagRange.getRange().contains(magnitude)) {
    magnitude = currentMagRange.snapToLegalValue(magnitude);
  }
  const float yNorm = currentMagRange.convertTo0to1(magnitude);
  return bounds.getBottom() - (yNorm * bounds.getHeight());
}
float FrameSpectrum::magForYPos(const frect_t& bounds, float yPos) const {
  auto yNorm = (yPos - bounds.getY()) / bounds.getHeight();
  return currentMagRange.convertFrom0to1(yNorm);
}

bool FrameSpectrum::isNearEditLine(const frect_t& bounds, fpoint_t point) {
  const float freq = xToNormFreq(bounds, point.x);
  float editY;
  if (warpPoints != -1) {
    editY = yPosWarped(bounds, freq);
  } else {
    editY = yPosLoaded(bounds, freq);
  }
  fpoint_t editPoint(point.x, editY);
  auto dist = point.getDistanceFrom(editPoint);
  return dist < MAX_CLICK_DISTANCE;
}

fpoint_t FrameSpectrum::warpPointToCanvas(const frect_t& bounds, WarpPoint* p) {
  float magnitude = p->warpedMag;
  if (!currentMagRange.getRange().contains(magnitude)) {
    magnitude = currentMagRange.snapToLegalValue(magnitude);
  }
  const float yNorm = currentMagRange.convertTo0to1(magnitude);
  const float y = bounds.getY() + (yNorm * bounds.getHeight());
  return {normFreqToXPos(bounds, p->normFreq), y};
}

static void s_drawEditHandle(juce::Graphics& g, fpoint_t center) {
  frect_t rect;
  rect.setCentre(center);
  rect = rect.withSizeKeepingCentre(12.0f, 12.0f);
  g.fillEllipse(rect);
}

void FrameSpectrum::paint(juce::Graphics& g) {
  // 1. fill the background
  auto fBounds = getLocalBounds().toFloat();
  g.setColour(UIColor::windowBkgnd);
  g.fillRect(fBounds);
  // 2. draw the loaded spectrum graph
  juce::Path pathLoaded;
  pathLoaded.startNewSubPath(fBounds.getX(), fBounds.getBottom());
  const float dX = fBounds.getWidth() / (float)FFT_GRAPH_RES;
  float x = fBounds.getX();
  float y;
  for (size_t i = 1; i < FFT_GRAPH_RES; ++i) {
    float freq = xToNormFreq(fBounds, x);
    y = yPosLoaded(fBounds, freq);
    pathLoaded.lineTo(x, y);
    x += dX;
  }
  pathLoaded.lineTo(fBounds.getWidth(), fBounds.getHeight());
  pathLoaded.closeSubPath();
  g.setColour(Color::literalOrangeBright.withAlpha(0.75f));
  g.fillPath(pathLoaded);
  // 3. if we have edit points, draw the shape of the warped wave first
  if (warp->numEditPoints() > 0) {
    juce::Path pathWarped;
    pathWarped.startNewSubPath(fBounds.getX(), fBounds.getBottom());
    x = fBounds.getX();
    for (size_t i = 0; i < FFT_GRAPH_RES; ++i) {
      float freq = xToNormFreq(fBounds, x);
      y = yPosWarped(fBounds, freq);
      pathWarped.lineTo(x, y);
      x += dX;
    }
    pathWarped.lineTo(fBounds.getWidth(), fBounds.getHeight());
    pathWarped.closeSubPath();
    g.setColour(Color::mintGreenBright.withAlpha(0.75f));
    g.fillPath(pathWarped);
    // 4. finally draw the edit points and handles
    g.setColour(Color::periwinkle);
    juce::Path editPath;
    editPath.startNewSubPath(warpPointToCanvas(fBounds, warp->getEditPoint(0)));
    for (int i = 0; i < warp->numEditPoints(); ++i) {
      auto* wp = warp->getEditPoint(i);
      auto point = warpPointToCanvas(fBounds, wp);
      editPath.lineTo(point);
      s_drawEditHandle(g, point);
    }
    juce::PathStrokeType pst(3.0f);
    g.strokePath(editPath, pst);
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
