#include "Electrum/GUI/WaveEditor/FrameWarp.h"
#include "Electrum/Audio/AudioUtil.h"
#include "Electrum/Audio/Wavetable.h"
#include "Electrum/GUI/GUITypedefs.h"
#include "Electrum/GUI/LookAndFeel/Color.h"
#include "Electrum/GUI/WaveEditor/EditValueTree.h"
#include "juce_graphics/juce_graphics.h"

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

//================================================

FrameWarp::FrameWarp(ValueTree& vt) : pWaveTree(&vt) {
  jassert(pWaveTree->hasType(WaveEdit::WAVE_FRAME));
  // load the FFT data into our array
  String waveStr = vt[WaveEdit::frameStringData];
  const float magnitudeMax = Wave::loadAudibleBins(waveStr, savedBins);
  const float magnitudeMid = Wave::getMedianBinMagnitude(savedBins);
  magnitudeRange = rangeWithCenter(0.0f, magnitudeMax, magnitudeMid);

  // copy these into the warped array to start
  for (size_t i = 0; i < AUDIBLE_BINS; ++i) {
    workingBins[i] = savedBins[i];
  }
  // add non-accessible start and end points
  float startMag = Wave::getMagnitudeAtNormFrequency(savedBins, 0.0f);
  float endMag = Wave::getMagnitudeAtNormFrequency(savedBins, 0.99999999f);

  points.push_back({0.0f, startMag});
  points.push_back({0.99999999f, endMag});
  // find and/or create the Warp child
  auto child = pWaveTree->getChildWithName(WaveEdit::FFT_WARP);
  if (!child.isValid()) {
    ValueTree tWarp(WaveEdit::FFT_WARP);
    pWaveTree->appendChild(tWarp, nullptr);
  } else {
    for (auto it = child.begin(); it != child.end(); ++it) {
      auto pointTree = *it;
      points.push_back(warp_point_t::fromValueTree(pointTree));
    }
  }
  // and as always make sure points are sorted by frequency
  sortPoints();
  //
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
  // otherwise we binary search
  size_t l = 0;
  size_t r = points.size() - 1;
  while (l < r) {
    size_t mid = l + ((r - l) / 2);
    if (points[mid].frequency <= freq && points[mid + 1].frequency > freq) {
      return {&points[mid], &points[mid + 1]};
    }
    if (points[mid].frequency < freq) {
      l = mid + 1;
    } else if (points[mid].frequency > freq) {
      r = mid - 1;
    }
  }
  jassert(false);
  return {&points[0], &points[1]};
}

void FrameWarp::createWarpPoint(float normFreq) {
  float mag;
  if (points.size() < 3) {
    mag = Wave::getMagnitudeAtNormFrequency(savedBins, normFreq);
  } else {
    mag = Wave::getMagnitudeAtNormFrequency(workingBins, normFreq);
  }

  warp_point_t pt{normFreq, mag};
  points.push_back(pt);
  auto vt = warp_point_t::toValueTree(pt);
  getWarpTree().appendChild(vt, nullptr);
  points.push_back(pt);
  sortPoints();
  triggerAsyncUpdate();
}

bool FrameWarp::isNearEditLine(float normMagnitude, float freq) const {
  float editMag;
  if (points.size() < 3) {
    editMag = Wave::getMagnitudeAtNormFrequency(savedBins, freq);
  } else {
    editMag = Wave::getMagnitudeAtNormFrequency(workingBins, freq);
  }
  float editMagNorm = magnitudeRange.convertTo0to1(editMag);
  static const float maxDistance = 1.0f / 15.0f;
  return std::fabs(normMagnitude - editMagNorm) <= maxDistance;
}

void FrameWarp::handleAsyncUpdate() {
  // this is where the 'workingBins' values get updated
  size_t leftIdx = 0;
  float lGain =
      points[leftIdx].magnitude /
      Wave::getMagnitudeAtNormFrequency(savedBins, points[leftIdx].frequency);
  float rGain = points[leftIdx + 1].magnitude /
                Wave::getMagnitudeAtNormFrequency(
                    savedBins, points[leftIdx + 1].frequency);

  for (size_t i = 0; i < AUDIBLE_BINS; ++i) {
    float nFreq = (float)i / (float)AUDIBLE_BINS;
    if (nFreq > points[leftIdx + 1].frequency) {
      ++leftIdx;
      lGain =
          points[leftIdx].magnitude / Wave::getMagnitudeAtNormFrequency(
                                          savedBins, points[leftIdx].frequency);
      rGain = points[leftIdx + 1].magnitude /
              Wave::getMagnitudeAtNormFrequency(savedBins,
                                                points[leftIdx + 1].frequency);
    }
    const float t = (nFreq - points[leftIdx].frequency) /
                    (points[leftIdx + 1].frequency - points[leftIdx].frequency);
    const float gain = flerp(lGain, rGain, t);
    workingBins[i].magnitude = savedBins[i].magnitude * gain;
  }
}

bool FrameWarp::canPointHaveFrequency(warp_point_t* point, float normFreq) {
  auto neighbors = getNeighborsForFreq(point->frequency);
  return (normFreq >= neighbors.left->frequency) &&
         (normFreq < neighbors.right->frequency);
}

void FrameWarp::placePoint(warp_point_t* point,
                           float normMagnitude,
                           float freq) {
  point->frequency = freq;
  point->magnitude = magnitudeRange.convertFrom0to1(normMagnitude);
  triggerAsyncUpdate();
}

warp_point_t* FrameWarp::editablePointNear(float normMagnitude, float freq) {
  auto n = getNeighborsForFreq(freq);
  const float mag = magnitudeRange.convertFrom0to1(normMagnitude);
  const float rDist = s_warpPointDistance(*n.right, {freq, mag});
  const float lDist = s_warpPointDistance(*n.left, {freq, mag});
  static const float clickDist = 1.0f / 15.0f;
  warp_point_t* closer = (rDist < lDist) ? n.right : n.left;
  if (closer == &points[0] || closer == &points.back()) {
    return nullptr;
  }
  if (std::min(rDist, lDist) < clickDist) {
    return closer;
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
    y0Norm = magnitudeRange.convertTo0to1(bins[leftBin].magnitude);
  } else {
    size_t bin = leftBin + 1;
    jassert(bin < AUDIBLE_BINS);
    auto leftNorm = magnitudeRange.convertTo0to1(bins[leftBin].magnitude);
    auto binNorm = magnitudeRange.convertTo0to1(bins[bin].magnitude);
    y0Norm = flerp(leftNorm, binNorm, fStartBin - (float)leftBin);
  }
  p.lineTo(imgBounds.getX(),
           imgBounds.getBottom() - (imgBounds.getHeight() * y0Norm));
  // 2. find the index of the bin to the right of the view;
  const size_t rightBin = std::min<size_t>(
      AudioUtil::fastFloor64(freqEnd * (float)AUDIBLE_BINS) + 1,
      AUDIBLE_BINS - 1);
  // 3. main loop to plot each of the visible bin points
  for (size_t b = leftBin + 1; b < rightBin; ++b) {
    float freq = (float)b / (float)AUDIBLE_BINS;
    float yNorm = magnitudeRange.convertTo0to1(bins[b].magnitude);
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
    yNormEnd = magnitudeRange.convertTo0to1(bins[rightBin].magnitude);
  } else {
    const size_t innerBin = rightBin - 1;
    auto innerNorm = magnitudeRange.convertTo0to1(bins[innerBin].magnitude);
    auto rightNorm = magnitudeRange.convertTo0to1(bins[rightBin].magnitude);
    yNormEnd = flerp(innerNorm, rightNorm, fEndBin - (float)innerBin);
  }
  p.lineTo(imgBounds.getRight(),
           imgBounds.getBottom() - (imgBounds.getHeight() * yNormEnd));
  p.lineTo(imgBounds.getRight(), imgBounds.getBottom());
  p.closeSubPath();
  return p;
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

void FrameWarp::drawEditPoints(juce::Graphics& g,
                               const frect_t& bounds,
                               float freqStart,
                               float freqEnd,
                               warp_point_t* selectedPt) {
  // 1. find the first point above the start frequency;
  const float freqToXScale = (freqEnd - freqStart);
  size_t rightIdx = 0;
  while (points[rightIdx].frequency < freqStart) {
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
    yNorm = magnitudeRange.convertTo0to1(points[leftIdx].magnitude);
    fpoint_t pt = {x * freqToXScale,
                   bounds.getBottom() - (bounds.getHeight() * yNorm)};
    handlePoints.push_back({pt, &points[leftIdx] == selectedPt});
  } else {
    const float nLeft = magnitudeRange.convertTo0to1(points[leftIdx].magnitude);
    const float nRight =
        magnitudeRange.convertTo0to1(points[rightIdx].magnitude);
    const float t = (freqStart - points[leftIdx].frequency) /
                    (points[rightIdx].frequency - points[leftIdx].frequency);
    yNorm = flerp(nLeft, nRight, t);
  }
  p.startNewSubPath(x * freqToXScale,
                    bounds.getBottom() - (bounds.getHeight() * yNorm));
  // 3. add points in the visible range to this path and the handle array
  while (points[rightIdx].frequency <= freqEnd && rightIdx < points.size()) {
    x = (points[rightIdx].frequency - freqStart) * freqToXScale;
    yNorm = magnitudeRange.convertTo0to1(points[rightIdx].magnitude);
    fpoint_t pt = {x, bounds.getBottom() - (bounds.getHeight() * yNorm)};
    handlePoints.push_back({pt, &points[rightIdx] == selectedPt});
    p.lineTo(pt);
    ++rightIdx;
  }
  // 4. if we haven't already drawn the last point, do the y-intercept thing
  // again
  if (rightIdx < points.size()) {
    const float nLeft =
        magnitudeRange.convertTo0to1(points[rightIdx - 1].magnitude);
    const float nRight =
        magnitudeRange.convertTo0to1(points[rightIdx].magnitude);
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
  g.strokePath(p, pst);
  // 6. draw the handles
  for (auto& h : handlePoints) {
    s_drawWarpHandle(g, h.point, h.selected);
  }
}

void FrameWarp::drawSpectrumRange(juce::Graphics& g,
                                  const frect_t& fBounds,
                                  float freqStart,
                                  float freqEnd,
                                  warp_point_t* selectedPt) {
  // 1. fill the background
  g.setColour(UIColor::windowBkgnd);
  g.fillRect(fBounds);
  //  2. draw the saved wave's spectrum
  auto savedPath = makeWindowedPath(savedBins, freqStart, freqEnd, fBounds);
  auto savedColor = Color::literalOrangePale.withAlpha(0.65f);
  g.setColour(savedColor);
  g.fillPath(savedPath);
  // 3. if we have an active warp, draw the warped spectrum and the
  //  edit points
  if (points.size() > 2) {
    auto workingPath =
        makeWindowedPath(workingBins, freqStart, freqEnd, fBounds);
    auto workingColor = Color::periwinkle.withAlpha(0.65f);
    g.setColour(workingColor);
    g.fillPath(workingPath);
    drawEditPoints(g, fBounds, freqStart, freqEnd, selectedPt);
  }
}
