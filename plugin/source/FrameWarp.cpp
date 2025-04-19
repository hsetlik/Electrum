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

static size_t freqToBinIdx(float freq) {
  float fIdx = freq * (float)(AUDIBLE_BINS - 1);
  return (size_t)fIdx;
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
  if (points.empty()) {
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
  if (!points.empty()) {
    for (size_t i = 0; i < AUDIBLE_BINS; ++i) {
      workingBins[i].magnitude = warpBinMagnitude(i);
    }
  } else {
    for (size_t i = 0; i < AUDIBLE_BINS; ++i) {
      workingBins[i] = savedBins[i];
    }
  }
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

warp_point_t* FrameWarp::closestEditPoint(size_t bin) {
  if (points.empty()) {
    return nullptr;
  }
  const float binFreq = binCenters[bin];
  // TODO: maybe binary search here
  // but I'll brute force for now to see if it
  //  works
  warp_point_t* closest = &points[0];
  float minDifference = 5000.0f;
  for (auto& point : points) {
    float diff = std::fabs(point.frequency - binFreq);
    if (diff < minDifference) {
      minDifference = diff;
      closest = &point;
    }
  }
  return closest;
}

size_t FrameWarp::minDistanceToNeutral(size_t ptIndex) {
  // 1. find the neutral point below
  size_t lNeutralBin;
  if (ptIndex < 1) {
    lNeutralBin = 0;
  } else {
    const float nFreq =
        flerp(points[ptIndex - 1].frequency, points[ptIndex].frequency, 0.5f);
    lNeutralBin = freqToBinIdx(nFreq);
  }
  // 2. find the neutral point above
  size_t rNeutralBin;
  if (ptIndex == points.size() - 1) {
    rNeutralBin = AUDIBLE_BINS - 1;
  } else {
    const float nFreq =
        flerp(points[ptIndex].frequency, points[ptIndex + 1].frequency, 0.5f);
    rNeutralBin = freqToBinIdx(nFreq);
  }
  const size_t ptBin = freqToBinIdx(points[ptIndex].frequency);
  const size_t lDist = ptBin - lNeutralBin;
  const size_t rDist = rNeutralBin - ptBin;
  return std::min<size_t>(lDist, rDist);
}

float FrameWarp::getInfluenceAmt(warp_point_t* point, size_t binIdx) {
  jassert(point != nullptr);
  const float binFreq = binCenters[binIdx];
  // 1. If we's at the same bin as the edit point,
  // the 'influence' is 100%
  if (fequal(binFreq, point->frequency)) {
    return 1.0f;
  }

  const size_t ptIndex = indexOf(point);
  const float fPointBin = (float)freqToBinIdx(point->frequency);
  const float fNeutralDist = (float)minDistanceToNeutral(ptIndex);
  const float fBinDist = std::fabs((float)binIdx - fPointBin);
  if (fBinDist >= fNeutralDist) {
    return 0.0f;
  }
  return 1.0f - (fBinDist / fNeutralDist);
}

float FrameWarp::warpBinMagnitude(size_t binIdx) {
  auto* editPoint = closestEditPoint(binIdx);
  if (editPoint == nullptr) {
    return savedBins[binIdx].magnitude;
  } else {
    const float warpAmt = getInfluenceAmt(editPoint, binIdx);
    return flerp(savedBins[binIdx].magnitude, editPoint->magnitude, warpAmt);
  }
}

//=====================================================================

warp_point_t* FrameWarp::editablePointNear(const frect_t& bounds,
                                           const fpoint_t& point,
                                           float thresh) {
  // 1. we just just skip if we don't have any edit points
  if (points.empty())
    return nullptr;
  // check the points to see what's closest
  for (auto& p : points) {
    auto uiPoint = warpPointToBounds(bounds, p);
    if (uiPoint.getDistanceFrom(point) < thresh) {
      return &p;
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

  // 2. figure out if the first point is on screen or not
  juce::Path p;
  float x = bounds.getX();
  float yNorm;
  std::vector<warp_handle_t> handlePoints = {};

  for (auto& point : points) {
    if (point.frequency >= freqStart && point.frequency < freqEnd) {
      x = (point.frequency - freqStart) * freqToXScale;
      jassert(magnitudeRange.getRange().contains(point.magnitude));
      yNorm = magnitudeToNorm(point.magnitude);
      float y = bounds.getBottom() - (bounds.getHeight() * yNorm);
      fpoint_t pCenter = {x, y};
      handlePoints.push_back({pCenter, &point == selectedPt});
    }
  }
  // 5. stroke the line
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
  if (!points.empty()) {
    drawBinsFixedColor(g, savedBins, fBounds, freqStart, freqEnd, savedColor);
    drawBinsPhaseColors(g, workingBins, fBounds, freqStart, freqEnd);
    drawEditPoints(g, fBounds, freqStart, freqEnd, selectedPt);
  } else {
    drawBinsPhaseColors(g, workingBins, fBounds, freqStart, freqEnd);
  }
}
