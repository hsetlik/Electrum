#include "Electrum/GUI/WaveEditor/FrameWarp.h"
#include "Electrum/Audio/AudioUtil.h"
#include "Electrum/Audio/Wavetable.h"
#include "Electrum/GUI/GUITypedefs.h"
#include "Electrum/GUI/LookAndFeel/Color.h"
#include "Electrum/GUI/WaveEditor/EditValueTree.h"
#include "juce_audio_basics/juce_audio_basics.h"
#include "juce_core/juce_core.h"
#include "juce_graphics/juce_graphics.h"

#define NAIVE_NEIGHBORS

warp_point_t warp_point_t::fromValueTree(const ValueTree& vt) {
  warp_point_t pt;
  jassert(vt.hasType(WaveEdit::FFT_GAIN_POINT));
  pt.frequency = vt[WaveEdit::gainPointFrequency];
  pt.magnitude = vt[WaveEdit::gainPointMagnitude];
  return pt;
}

ValueTree warp_point_t::toValueTree(const warp_point_t& pt) {
  ValueTree tree(WaveEdit::FFT_GAIN_POINT);
  tree.setProperty(WaveEdit::gainPointMagnitude, pt.magnitude, nullptr);
  tree.setProperty(WaveEdit::gainPointFrequency, pt.frequency, nullptr);
  return tree;
}

static float s_warpPointDistance(const warp_point_t& a, const warp_point_t& b) {
  const float dFreq = a.frequency - b.frequency;
  const float dMag = a.magnitude - b.magnitude;
  return std::sqrtf((dFreq * dFreq) + (dMag * dMag));
}

static std::array<float, AUDIBLE_BINS> s_genBinCenterFreqs() {
  std::array<float, AUDIBLE_BINS> centers = {};
  for (size_t i = 0; i < AUDIBLE_BINS; ++i) {
    centers[i] = (float)i / (float)AUDIBLE_BINS;
  }
  return centers;
}

static std::array<float, AUDIBLE_BINS> binCenters = s_genBinCenterFreqs();
static float snapToBinCenter(float nFreq) {
  float fIdx = nFreq * (float)(AUDIBLE_BINS - 1);
  return binCenters[(size_t)fIdx];
}

//================================================

FrameWarp::FrameWarp(ValueTree& vt) : loadedWaveTree(vt.createCopy()) {
  // load the FFT data into our array
  String waveStr = vt[WaveEdit::frameStringData];
  const float magnitudeMax = Wave::loadAudibleBins(waveStr, savedBins);
  const float magnitudeMid = Wave::getMedianBinMagnitude(savedBins);
  static const float magHeadroom = juce::Decibels::decibelsToGain(4.0f);
  maxMagnitude = magnitudeMax * magHeadroom;
  magnitudeRange = rangeWithCenter(0.0f, maxMagnitude, magnitudeMid);

  // copy these into the warped array to start
  for (size_t i = 0; i < AUDIBLE_BINS; ++i) {
    workingBins[i] = savedBins[i];
  }
  // add non-accessible start and end points
  float startMag = Wave::getMagnitudeAtNormFrequency(savedBins, 0.0f);
  float endMag = Wave::getMagnitudeAtNormFrequency(savedBins, 1.0f);

  points.push_back({0.0f, startMag});
  points.push_back({1.0f, endMag});
  // find and/or create the Warp child
  auto child = loadedWaveTree.getChildWithName(WaveEdit::FFT_WARP);
  if (!child.isValid()) {
    ValueTree tWarp(WaveEdit::FFT_WARP);
    loadedWaveTree.appendChild(tWarp, nullptr);

  } else {
    for (auto it = child.begin(); it != child.end(); ++it) {
      auto pointTree = *it;
      points.push_back(warp_point_t::fromValueTree(pointTree));
    }
  }
  // and as always make sure points are sorted by frequency
  sortPoints();
  binsReady = true;
}

void FrameWarp::sortPoints() {
  std::sort(points.begin(), points.end(),
            [](warp_point_t a, warp_point_t b) { return a < b; });
}

FrameWarp::neighbor_pair_t FrameWarp::getNeighborsForFreq(float freq) {
  // if we haven't added any warping, this will always just be the first
  // and last point
  if (points.size() < 3)
    return {&points[0], &points[1]};
#ifndef NAIVE_NEIGHBORS
  // otherwise we binary search
  size_t l = 0;
  size_t r = points.size() - 1;
  while (l < r) {
    size_t mid = l + ((r - l) / 2);
    if (points[mid].frequency <= freq && points[mid + 1].frequency > freq) {
      return {&points[mid], &points[mid + 1]};
    }
    if (points[mid].frequency < freq) {
      l = mid;
    } else if (points[mid].frequency >= freq) {
      r = mid;
    }
  }
#else
  for (size_t i = 1; i < points.size(); ++i) {
    auto* l = &points[i - 1];
    auto* r = &points[i];
    if (l->frequency < freq && r->frequency >= freq) {
      return {l, r};
    }
  }
#endif
  jassert(false);
  return {&points[0], &points[1]};
}

void FrameWarp::createWarpPoint(float normMag, float normFreq) {
  if (!binsReady)
    return;
  const float mag = magnitudeRange.convertFrom0to1(normMag);
  normFreq = snapToBinCenter(normFreq);
  warp_point_t pt{normFreq, mag};
  const float minDistance = 1.0f / 25.0f;
  for (auto& point : points) {
    if (s_warpPointDistance(pt, point) < minDistance)
      return;
  }
  points.push_back(pt);
  auto vt = warp_point_t::toValueTree(pt);
  getWarpTree().appendChild(vt, nullptr);
  sortPoints();
  triggerAsyncUpdate();
}

bool FrameWarp::isNearEditLine(float normMagnitude, float freq) const {
  float editMag;
  if (points.size() < 3) {
    editMag = Wave::getMagnitudeAtNormFrequency(savedBins, freq);
    jassert(magnitudeRange.getRange().contains(editMag));
  } else {
    editMag = Wave::getMagnitudeAtNormFrequency(workingBins, freq);
    jassert(magnitudeRange.getRange().contains(editMag));
  }
  float editMagNorm = magnitudeToNorm(editMag);
  static const float maxDistance = 1.0f / 15.0f;
  return std::fabs(normMagnitude - editMagNorm) <= maxDistance;
}

void FrameWarp::handleAsyncUpdate() {
  juce::ScopedLock sl(criticalSection);
  // this is where the 'workingBins' values get updated
  size_t leftIdx = 0;
  if (points.size() > 2) {
    size_t rightBin =
        (size_t)(points[leftIdx + 1].frequency * (float)(AUDIBLE_BINS - 1));
    for (size_t i = 0; i < AUDIBLE_BINS; ++i) {
      if (i > rightBin) {
        ++leftIdx;
        jassert((leftIdx + 1) < points.size());
        rightBin =
            (size_t)(points[leftIdx + 1].frequency * (float)(AUDIBLE_BINS - 1));
      }
      workingBins[i].magnitude = getWarpedBinMagnitude(leftIdx, i);
    }
  } else {
    for (size_t i = 0; i < AUDIBLE_BINS; ++i) {
      workingBins[i] = savedBins[i];
    }
  }
}

float FrameWarp::getWarpedBinMagnitude(size_t leftPointIdx, size_t binIdx) {
  // 1. find the original magnitude of the point
  const float savedMag = savedBins[binIdx].magnitude;
  const float binFreq = (float)binIdx / (float)AUDIBLE_BINS;
  // 2. find the bin indices for the two warp points
  const size_t leftBin =
      (size_t)(points[leftPointIdx].frequency * (float)(AUDIBLE_BINS - 1));
  const size_t rightBin =
      (size_t)(points[leftPointIdx + 1].frequency * (float)(AUDIBLE_BINS - 1));
  const float t =
      (binFreq - points[leftPointIdx].frequency) /
      (points[leftPointIdx + 1].frequency - points[leftPointIdx].frequency);
  // 2. find the gain at each point
  const float lGain =
      points[leftPointIdx].magnitude / savedBins[leftBin].magnitude;
  const float rGain =
      points[leftPointIdx + 1].magnitude / savedBins[rightBin].magnitude;
  const float mag = savedMag * flerp(lGain, rGain, t);
  return magnitudeRange.snapToLegalValue(mag);
}

void FrameWarp::placePoint(warp_point_t* point,
                           float normMagnitude,
                           float freq) {
  if (isMovementLegal(point, normMagnitude, freq)) {
    point->frequency = snapToBinCenter(freq);
    point->magnitude = magnitudeRange.convertFrom0to1(normMagnitude);
    triggerAsyncUpdate();
  }
}

warp_point_t* FrameWarp::editablePointNear(float normMagnitude, float freq) {
  auto n = getNeighborsForFreq(freq);
  const float mag = magnitudeToNorm(normMagnitude);
  const float rDist = s_warpPointDistance(*n.right, {freq, mag});
  const float lDist = s_warpPointDistance(*n.left, {freq, mag});
  static const float clickDist = 1.0f / 10.0f;
  warp_point_t* closer = (rDist < lDist) ? n.right : n.left;
  if (closer == &points[0] || closer == &points.back()) {
    return nullptr;
  }
  if (std::min(rDist, lDist) < clickDist) {
    return closer;
  }
  return nullptr;
}

size_t FrameWarp::indexOf(warp_point_t* pt) const {
  for (size_t i = 0; i < points.size(); ++i) {
    if (&points[i] == pt)
      return i;
  }
  jassert(false);
  return 0;
}

bool FrameWarp::isMovementLegal(warp_point_t* pt,
                                float normMagnitude,
                                float freq) const {
  auto idx = indexOf(pt);
  if (idx == 0 || idx == (points.size() - 1))
    return false;
  auto& left = points[idx - 1];
  auto& right = points[idx + 1];
  if (freq < left.frequency || freq > right.frequency) {
    return false;
  }
  if (normMagnitude < 0.0f || normMagnitude > 1.0f)
    return false;
  return true;
}

fpoint_t FrameWarp::warpPointToBounds(const frect_t& bounds,
                                      const warp_point_t& wp) const {
  const float xPos = bounds.getX() + (bounds.getWidth() * wp.frequency);
  const float normMag = magnitudeToNorm(wp.magnitude);
  const float yPos = bounds.getBottom() - (bounds.getHeight() * normMag);
  return {xPos, yPos};
}

warp_point_t* FrameWarp::editablePointNear(const frect_t& bounds,
                                           const fpoint_t& point,
                                           float thresh) {
  // 1. convert the point into frequency
  const float clickFreq = (point.x - bounds.getX()) / bounds.getWidth();
  // 2. find the neighbors
  auto neighbors = getNeighborsForFreq(clickFreq);
  // 3. check if each is legal, then translate back to bounds space and
  // check the distance
  if (neighbors.left != &points[0]) {
    auto lPoint = warpPointToBounds(bounds, *neighbors.left);
    if (point.getDistanceFrom(lPoint) < thresh) {
      return neighbors.left;
    }
  }
  if (neighbors.right != &points.back()) {
    auto rPoint = warpPointToBounds(bounds, *neighbors.right);
    if (point.getDistanceFrom(rPoint) < thresh) {
      return neighbors.right;
    }
  }
  return nullptr;
}

static void s_removePoint(std::vector<warp_point_t>& vec, warp_point_t* point) {
  for (auto it = vec.begin(); it != vec.end(); ++it) {
    auto& pt = *it;
    if (&pt == point) {
      vec.erase(it);
      return;
    }
  }
  return;
}

void FrameWarp::deletePoint(warp_point_t* point) {
  s_removePoint(points, point);
  sortPoints();
  triggerAsyncUpdate();
}
//===================================================
// helper for path drawing
juce::Path FrameWarp::makeWindowedPath(const bin_array_t& bins,
                                       float freqStart,
                                       float freqEnd,
                                       const frect_t& imgBounds) {
  // all the frequencies are normalized, so scaling to the x axis is easy
  const float freqToXScale = imgBounds.getWidth() / (freqEnd - freqStart);
  juce::Path p;
  p.startNewSubPath(imgBounds.getX(), imgBounds.getBottom());
  // 1. find the bin just left of the window and its y intercept for the left
  // edge
  const float fStartBin = freqStart * (float)AUDIBLE_BINS;
  const size_t leftBin = AudioUtil::fastFloor64(fStartBin);
  float y0Norm;
  if (fequal(fStartBin, (float)leftBin)) {
    if (&bins == &workingBins) {
      jassert(magnitudeRange.getRange().contains(bins[leftBin].magnitude));
    } else {
      jassert(magnitudeRange.getRange().contains(bins[leftBin].magnitude));
    }
    y0Norm = magnitudeToNorm(bins[leftBin].magnitude);
  } else {
    size_t bin = leftBin + 1;
    jassert(bin < AUDIBLE_BINS);
    jassert(magnitudeRange.getRange().contains(bins[leftBin].magnitude));
    auto leftNorm = magnitudeToNorm(bins[leftBin].magnitude);
    jassert(magnitudeRange.getRange().contains(bins[bin].magnitude));
    auto binNorm = magnitudeToNorm(bins[bin].magnitude);
    y0Norm = flerp(leftNorm, binNorm, fStartBin - (float)leftBin);
  }
  p.lineTo(imgBounds.getX(),
           imgBounds.getBottom() - (imgBounds.getHeight() * y0Norm));
  // 2. find the index of the bin to the right of the view;
  const size_t rightBin =
      AudioUtil::fastFloor64(freqEnd * (float)(AUDIBLE_BINS - 1));
  jassert(rightBin < AUDIBLE_BINS);

  // 3. main loop to plot each of the visible bin points
  for (size_t b = leftBin + 1; b < rightBin; ++b) {
    float freq = (float)b / (float)AUDIBLE_BINS;

    if (&bins == &savedBins) {
      jassert(magnitudeRange.getRange().contains(bins[b].magnitude));
    }
    float yNorm = magnitudeToNorm(bins[b].magnitude);
    float x = (freq - freqStart) * freqToXScale;
    p.lineTo(imgBounds.getX() + x,
             imgBounds.getBottom() - (imgBounds.getHeight() * yNorm));
  }
  // 4. similar to above, find the first out-of-frame bin to the right
  // and figure out the y-intercept
  float fEndBin = freqEnd * (float)AUDIBLE_BINS;
  float yNormEnd;
  if (fEndBin <= (float)rightBin) {
    // if the right bin is in frame, we can just draw it
    // normally
    yNormEnd = magnitudeToNorm(bins[rightBin].magnitude);
  } else {
    const size_t innerBin = rightBin - 1;

    jassert(magnitudeRange.getRange().contains(bins[innerBin].magnitude));
    auto innerNorm = magnitudeToNorm(bins[innerBin].magnitude);
    jassert(magnitudeRange.getRange().contains(bins[rightBin].magnitude));
    auto rightNorm = magnitudeToNorm(bins[rightBin].magnitude);
    yNormEnd = flerp(innerNorm, rightNorm, fEndBin - (float)innerBin);
  }
  p.lineTo(imgBounds.getRight(),
           imgBounds.getBottom() - (imgBounds.getHeight() * yNormEnd));
  p.lineTo(imgBounds.getRight(), imgBounds.getBottom());
  p.closeSubPath();
  return p;
}

std::vector<bin_area_t> FrameWarp::getVisibleBinAreas(const bin_array_t& bins,
                                                      float fStart,
                                                      float fEnd,
                                                      const frect_t& bounds) {
  std::vector<bin_area_t> vec;
  const float fStartBin = fStart * (float)(AUDIBLE_BINS - 1);
  const float fEndBin = fEnd * (float)(AUDIBLE_BINS - 1);
  const float binWidth = bounds.getWidth() / (fEndBin - fStartBin);
  const size_t firstBin = AudioUtil::fastFloor64(fStartBin);
  auto bin = firstBin;
  while (binCenters[bin] < fEnd) {
    float x = bounds.getX() + (((float)bin - fStartBin) * binWidth) -
              (binWidth / 2.0f);
    float magNorm = magnitudeToNorm(bins[bin].magnitude);
    float height = bounds.getHeight() * magNorm;
    frect_t binArea = {x, bounds.getBottom() - height, binWidth, height};
    vec.push_back({binArea, bin});
    ++bin;
  }
  return vec;
}

static void s_drawWarpHandle(juce::Graphics& g,
                             const fpoint_t& center,
                             bool isSelected) {
  frect_t bounds;
  bounds.setCentre(center);
  static const float handleWidth = 13.0f;
  bounds = bounds.withSizeKeepingCentre(handleWidth, handleWidth);
  static color_t outline = Color::mintGreenPale.darker(0.5f);
  static color_t fill = Color::mintGreenPale;
  g.setColour(fill);
  g.fillEllipse(bounds);
  if (isSelected) {
    g.setColour(outline);
    g.drawEllipse(bounds, 2.5f);
  }
}

static void s_drawWarpHandleScaled(juce::Graphics& g,
                                   const fpoint_t& center,
                                   bool isSelected,
                                   float handleWidth) {
  frect_t bounds;
  bounds.setCentre(center);
  bounds = bounds.withSizeKeepingCentre(handleWidth, handleWidth);
  static color_t outline = Color::mintGreenPale.darker(0.5f);
  static color_t fill = Color::mintGreenPale;
  g.setColour(fill);
  g.fillEllipse(bounds);
  if (isSelected) {
    g.setColour(outline);
    g.drawEllipse(bounds, 2.5f);
  }
}

void FrameWarp::drawEditPoints(juce::Graphics& g,
                               const frect_t& bounds,
                               float freqStart,
                               float freqEnd,
                               warp_point_t* selectedPt) {
  // 1. find the first point above the start frequency;
  const float freqToXScale = bounds.getWidth() / (freqEnd - freqStart);
  size_t rightIdx = 0;
  while (points[rightIdx].frequency <= freqStart) {
    ++rightIdx;
  }
  // 2. figure out if the first point is on screen or not
  juce::Path p;
  jassert(rightIdx > 0);
  size_t leftIdx = rightIdx - 1;
  float x = bounds.getX();
  float yNorm;
  std::vector<warp_handle_t> handlePoints = {};
  if (points[leftIdx].frequency <= freqStart) {
    // we can draw the first point as normal
    yNorm = magnitudeToNorm(points[leftIdx].magnitude);
    fpoint_t pt = {x * freqToXScale,
                   bounds.getBottom() - (bounds.getHeight() * yNorm)};
    handlePoints.push_back({pt, &points[leftIdx] == selectedPt});
  } else {
    jassert(magnitudeRange.getRange().contains(points[leftIdx].magnitude));
    const float nLeft = magnitudeToNorm(points[leftIdx].magnitude);
    jassert(magnitudeRange.getRange().contains(points[rightIdx].magnitude));
    const float nRight = magnitudeToNorm(points[rightIdx].magnitude);
    const float t = (freqStart - points[leftIdx].frequency) /
                    (points[rightIdx].frequency - points[leftIdx].frequency);
    yNorm = flerp(nLeft, nRight, t);
  }
  p.startNewSubPath(x * freqToXScale,
                    bounds.getBottom() - (bounds.getHeight() * yNorm));
  // 3. add points in the visible range to this path and the handle array
  while (points[rightIdx].frequency <= freqEnd && rightIdx < points.size()) {
    x = (points[rightIdx].frequency - freqStart) * freqToXScale;
    jassert(magnitudeRange.getRange().contains(points[rightIdx].magnitude));
    yNorm = magnitudeToNorm(points[rightIdx].magnitude);
    fpoint_t pt = {x, bounds.getBottom() - (bounds.getHeight() * yNorm)};
    handlePoints.push_back({pt, &points[rightIdx] == selectedPt});
    p.lineTo(pt);
    ++rightIdx;
  }
  // 4. if we haven't already drawn the last point, do the y-intercept thing
  // again
  if (rightIdx < points.size()) {
    jassert(magnitudeRange.getRange().contains(points[rightIdx - 1].magnitude));
    const float nLeft = magnitudeToNorm(points[rightIdx - 1].magnitude);
    jassert(magnitudeRange.getRange().contains(points[rightIdx].magnitude));
    const float nRight = magnitudeToNorm(points[rightIdx].magnitude);
    const float t =
        (freqEnd - points[rightIdx - 1].frequency) /
        (points[rightIdx].frequency - points[rightIdx - 1].frequency);
    yNorm = flerp(nLeft, nRight, t);
    p.lineTo(bounds.getRight(),
             bounds.getBottom() - (bounds.getHeight() * yNorm));
  }
  // 5. stroke the line
  g.setColour(Color::mintGreenPale);
  juce::PathStrokeType pst(2.0f);
  // g.strokePath(p, pst);
  //  6. draw the handles
  const float fStartBin = freqStart * (float)(AUDIBLE_BINS - 1);
  const float fEndBin = freqEnd * (float)(AUDIBLE_BINS - 1);
  const float binWidth = bounds.getWidth() / (fEndBin - fStartBin);
  for (auto& h : handlePoints) {
    s_drawWarpHandleScaled(g, h.point, h.selected, binWidth);
  }
}

void FrameWarp::drawBinsFixedColor(juce::Graphics& g,
                                   const bin_array_t& bins,
                                   const frect_t& bounds,
                                   float fStart,
                                   float fEnd,
                                   const color_t& color) {
  auto areas = getVisibleBinAreas(bins, fStart, fEnd, bounds);
  g.setColour(color);
  for (auto& a : areas) {
    g.fillRect(a.bounds);
  }
}

void FrameWarp::drawBinsPhaseColors(juce::Graphics& g,
                                    const bin_array_t& bins,
                                    const frect_t& bounds,
                                    float fStart,
                                    float fEnd) {
  auto areas = getVisibleBinAreas(bins, fStart, fEnd, bounds);
  auto colorA = Color::darkSeaGreen;
  auto colorB = Color::literalOrangePale;
  for (auto& a : areas) {
    auto normPhase = bins[a.idx].phase / juce::MathConstants<float>::twoPi;
    auto color = colorA.interpolatedWith(colorB, normPhase);
    g.setColour(color);
    g.fillRect(a.bounds);
  }
}

float FrameWarp::magnitudeToNorm(float mag) const {
  auto safeMag = std::min(maxMagnitude, mag);
  return magnitudeRange.convertTo0to1(safeMag);
}
void FrameWarp::drawSpectrumRange(juce::Graphics& g,
                                  const frect_t& fBounds,
                                  float freqStart,
                                  float freqEnd,
                                  warp_point_t* selectedPt) {
  // 1. fill the background
  g.setColour(UIColor::windowBkgnd);
  g.fillRect(fBounds);
  static color_t savedColor = Color::literalOrangePale.withAlpha(0.85f);
  if (points.size() > 2) {
    drawBinsFixedColor(g, savedBins, fBounds, freqStart, freqEnd, savedColor);
    drawBinsPhaseColors(g, workingBins, fBounds, freqStart, freqEnd);
    drawEditPoints(g, fBounds, freqStart, freqEnd, selectedPt);
  } else {
    drawBinsPhaseColors(g, workingBins, fBounds, freqStart, freqEnd);
  }
}
