#include "Electrum/Shared/PointwiseWave.h"
#include "Electrum/Audio/AudioUtil.h"
#include "Electrum/Audio/Wavetable.h"
#include "Electrum/GUI/LookAndFeel/Color.h"
#include "Electrum/GUI/WaveEditor/EditValueTree.h"

static bool s_compareWavePts(wave_point_t a, wave_point_t b) {
  return a.waveIdx < b.waveIdx;
}

namespace Pointwise {
static const float headroomDbAbs = 4.0f;
static const float yHeadroomNeg =
    juce::Decibels::decibelsToGain(-headroomDbAbs);
static const float yHeadroomPos = juce::Decibels::decibelsToGain(headroomDbAbs);

wave_point_t projectSpaceToWavePoint(const frect_t& bounds,
                                     const fpoint_t& point) {
  const float lvlNorm = (bounds.getBottom() - point.y) / bounds.getHeight();
  const float xNorm = (point.x - bounds.getX()) / bounds.getWidth();
  const int waveIdx = (int)(xNorm * (float)(TABLE_SIZE - 1));
  const float lvl = flerp(-1.0f, 1.0f, 1.0f - lvlNorm);
  return {waveIdx, lvl / yHeadroomNeg, false, 0};
}
fpoint_t projectWavePointToSpace(const frect_t& bounds,
                                 const wave_point_t& point) {
  const float y0 = bounds.getY() + (bounds.getHeight() / 2.0f);
  const float yAmplitude = (y0 - bounds.getY()) * yHeadroomNeg;
  const float xNorm = (float)point.waveIdx / (float)(TABLE_SIZE - 1);
  const float x = bounds.getX() + (xNorm * bounds.getWidth());
  return {x, y0 + (point.level * yAmplitude)};
}

//-------------------------------------------
ValueTree wavePointToTree(const wave_point_t& point) {
  ValueTree vt(WaveEdit::WAVE_POINT);
  vt.setProperty(WaveEdit::pointWaveIdx, point.waveIdx, nullptr);
  vt.setProperty(WaveEdit::pointLevel, point.level, nullptr);
  vt.setProperty(WaveEdit::pointLockedOnX, point.xAxisLocked, nullptr);
  vt.setProperty(WaveEdit::pointType, point.pointType, nullptr);
  if (point.pointType > 0) {
    ValueTree leftBez(WaveEdit::POINT_BEZIER_LEFT);
    leftBez.setProperty(WaveEdit::pointBezierLength, point.leftBezLength,
                        nullptr);
    leftBez.setProperty(WaveEdit::pointBezierTheta, point.leftBezTheta,
                        nullptr);

    ValueTree rightBez(WaveEdit::POINT_BEZIER_LEFT);
    rightBez.setProperty(WaveEdit::pointBezierLength, point.rightBezLength,
                         nullptr);
    rightBez.setProperty(WaveEdit::pointBezierTheta, point.rightBezTheta,
                         nullptr);

    vt.appendChild(leftBez, nullptr);
    vt.appendChild(rightBez, nullptr);
  }
  return vt;
}
wave_point_t treeToWavePoint(const ValueTree& tree) {
  wave_point_t wp;
  jassert(tree.hasType(WaveEdit::WAVE_POINT));
  wp.waveIdx = tree[WaveEdit::pointWaveIdx];
  wp.level = tree[WaveEdit::pointLevel];
  wp.xAxisLocked = tree[WaveEdit::pointLockedOnX];
  wp.pointType = tree[WaveEdit::pointType];
  return wp;
}

ValueTree wavePointsToValueTree(const wave_pt_vec& points) {
  ValueTree vt(WaveEdit::POINTWISE_WARP);
  for (auto& p : points) {
    vt.appendChild(wavePointToTree(p), nullptr);
  }
  return vt;
}

wave_pt_vec valueTreeToWavePoints(const ValueTree& warpTree) {
  jassert(warpTree.hasType(WaveEdit::POINTWISE_WARP));
  wave_pt_vec vec = {};
  for (auto it = warpTree.begin(); it != warpTree.end(); ++it) {
    auto child = *it;
    vec.push_back(treeToWavePoint(child));
  }
  std::sort(vec.begin(), vec.end(), s_compareWavePts);
  return vec;
}

//-------------------------------------------
// get the point at the end of a linear section of
// the wave, starting from this index
static int endOfLinearSection(float* wave, int startIdx) {
  jassert(startIdx < TABLE_SIZE - 1);
  int idx = startIdx;
  const float startSlope = wave[startIdx + 1] - wave[startIdx];
  float slope = wave[idx + 1] - wave[idx];
  while (fequal(slope, startSlope) && idx < TABLE_SIZE - 1) {
    ++idx;
    slope = wave[idx + 1] - wave[idx];
  }
  return idx;
}
wave_pt_vec parseWaveLinear(float* wave) {
  wave_pt_vec vec = {};
  parseWaveLinear(wave, vec);
  return vec;
}

void parseWaveLinear(float* wave, wave_pt_vec& vec) {
  vec.clear();
  float startLvl = wave[0];
  vec.push_back({0, startLvl, true, 0});
  int nextCorner = endOfLinearSection(wave, 0);
  while (nextCorner < TABLE_SIZE - 1) {
    float lvl = wave[nextCorner];
    vec.push_back({nextCorner, lvl, false, 0});
    nextCorner = endOfLinearSection(wave, nextCorner);
  }
  float endLvl = wave[TABLE_SIZE - 1];
  vec.push_back({TABLE_SIZE - 1, endLvl, true, 0});
}

//===================================================

int Warp::indexOf(wave_point_t* pt) const {
  return (int)closestPointIndex(pt->waveIdx);
}

size_t Warp::closestPointIndex(int waveIdx) const {
  size_t left = 0;
  size_t right = points.size() - 1;
  size_t closestIdx = 0;
  while (left <= right) {
    auto mid = left + ((right - left) / 2);
    auto& prevPt = points[closestIdx];
    auto& currentPt = points[mid];
    auto pDiff = std::abs(prevPt.waveIdx - waveIdx);
    auto cDiff = std::abs(currentPt.waveIdx - waveIdx);
    if (cDiff < pDiff) {
      closestIdx = mid;
    } else if (cDiff == pDiff) {
      closestIdx = std::max(closestIdx, mid);
    }

    // if we landed right on a point just return it now
    if (currentPt.waveIdx == waveIdx) {
      return mid;
    } else if (currentPt.waveIdx < waveIdx) {
      left = mid + 1;
    } else {
      right = mid - 1;
    }
  }
  return closestIdx;
}

void Warp::sortPoints() {
  std::sort(points.begin(), points.end(), s_compareWavePts);
}

size_t Warp::leftNeighborPointIndex(int waveIdx) const {
  auto closest = closestPointIndex(waveIdx);
  if (points[closest].waveIdx <= waveIdx) {
    return closest;
  } else {
    jassert(closest > 0);
    return closest - 1;
  }
}

//----------------------------------------------

bool Warp::pointExistsAt(int waveIdx) const {
  auto& nearest = points[closestPointIndex(waveIdx)];
  return nearest.waveIdx == waveIdx;
}

bool Warp::canCreatePoint(int waveIdx, float lvl) const {
  if (pointExistsAt(waveIdx))
    return false;
  if (lvl < -1.0f || lvl > 1.0f)
    return false;
  return waveIdx > 0 && waveIdx < TABLE_SIZE - 1;
}

bool Warp::pointCanMoveTo(wave_point_t* pt, int waveIdx, float lvl) const {
  if (lvl < -1.0f || lvl > 1.0f)
    return false;
  if (pt->xAxisLocked) {
    return waveIdx == pt->waveIdx;
  }
  size_t pIdx = closestPointIndex(pt->waveIdx);
  jassert(pIdx > 0 && pIdx < (points.size() - 1));
  auto& lPoint = points[pIdx - 1];
  auto& rPoint = points[pIdx + 1];
  return (waveIdx > lPoint.waveIdx) && (waveIdx < rPoint.waveIdx);
}

Warp::Warp(const String& frameStr) {
  // 1. parse the wave into the data array
  stringDecodeWave(frameStr, loadedWave);
  // 2. parse that into the points vector
  parseWaveLinear(loadedWave, points);
  jassert(points.size() > 1);
}

String Warp::encodeFrameString() const {
  float data[TABLE_SIZE];
  size_t leftPt, rightPt;
  for (int i = 0; i < TABLE_SIZE; ++i) {
    leftPt = leftNeighborPointIndex(i);
    rightPt = leftPt + 1;
    jassert(rightPt < points.size());
    const float n = (float)(i - points[leftPt].waveIdx) /
                    (float)(points[rightPt].waveIdx - points[leftPt].waveIdx);
    data[i] = flerp(points[leftPt].level, points[rightPt].level, n);
  }
  return stringEncodeWave(data);
}

void Warp::movePointConstrained(wave_point_t* pt, int waveIdx, float lvl) {
  pt->level = std::clamp(lvl, -1.0f, 1.0f);
  if (!pt->xAxisLocked) {
    auto pIdx = closestPointIndex(pt->waveIdx);
    auto nLeft = pIdx - 1;
    auto nRight = pIdx + 1;
    jassert(nRight < points.size());
    const int minWaveIdx = points[nLeft].waveIdx + 1;
    const int maxWaveIdx = points[nRight].waveIdx - 1;
    pt->waveIdx = std::clamp(waveIdx, minWaveIdx, maxWaveIdx);
  }
  waveTouched = true;
}

size_t Warp::createPoint(int waveIdx, float lvl, int wType) {
  if (canCreatePoint(waveIdx, lvl)) {
    wave_point_t newPoint = {waveIdx, lvl, false, wType};
    points.push_back(newPoint);
    sortPoints();
    waveTouched = true;
  }
  return 1;
}

bool Warp::deletePoint(size_t pointIdx) {
  if (pointIdx == 0 || pointIdx == points.size() - 1) {
    return false;
  }
  if (!points[pointIdx].xAxisLocked) {
    auto iDelete = std::next(points.begin(), (int)pointIdx);
    points.erase(iDelete);
    waveTouched = true;
  }
  return true;
}

// Selection stuff--------------------------
bool Warp::isPointSelected(const wave_point_t* pt) const {
  for (auto* s : selectedPoints) {
    if (s == pt)
      return true;
  }
  return false;
}

void Warp::selectPoint(wave_point_t* pt) {
  if (!isPointSelected(pt) && !pt->xAxisLocked) {
    selectedPoints.push_back(pt);
    lastSelectedPt = pt;
    waveTouched = true;
  }
}

static void s_erasePointerIn(std::vector<wave_point_t*> selected,
                             wave_point_t* pt) {
  for (auto it = selected.begin(); it != selected.end(); ++it) {
    if (*it == pt) {
      selected.erase(it);
      return;
    }
  }
  jassert(false);
}

void Warp::deselectPoint(wave_point_t* pt) {
  if (lastSelectedPt == pt)
    lastSelectedPt = nullptr;
  s_erasePointerIn(selectedPoints, pt);
  if (lastSelectedPt == nullptr && !selectedPoints.empty()) {
    lastSelectedPt = selectedPoints.back();
  }
  waveTouched = true;
}

void Warp::clearSelection() {
  lastSelectedPt = nullptr;
  selectedPoints.clear();
  waveTouched = true;
}

// Mouse Handling------------------------------------------------
wave_point_t* Warp::getNearbyPoint(const frect_t& bounds,
                                   const fpoint_t& point,
                                   float thresh) {
  for (auto& editPoint : points) {
    auto other = projectWavePointToSpace(bounds, editPoint);
    if (other.getDistanceFrom(point) < thresh) {
      return &editPoint;
    }
  }
  return nullptr;
}

bool Warp::dragUpdateAllowed(const wave_point_t& newPoint) const {
  const float dLevel = newPoint.level - lastDragUpdatePoint.level;
  const int dIndex = newPoint.waveIdx - lastDragUpdatePoint.waveIdx;

  for (auto* sp : selectedPoints) {
    float newLevel = sp->level + dLevel;
    int newIdx = sp->waveIdx + dIndex;
    if (!pointCanMoveTo(sp, newIdx, newLevel)) {
      return false;
    }
  }
  return true;
}

bool Warp::dragUpdateAllowed(const frect_t& bounds,
                             const fpoint_t& newPoint) const {
  if (selectedPoints.size() > 2)
    return true;
  auto prevPoint = projectWavePointToSpace(bounds, lastDragUpdatePoint);
  const float dX = newPoint.x - prevPoint.x;
  const float dY = newPoint.y - prevPoint.y;
  for (auto* sp : selectedPoints) {
    auto pSpace = projectWavePointToSpace(bounds, *sp);
    pSpace.x += dX;
    pSpace.y += dY;
    auto pWave = projectSpaceToWavePoint(bounds, pSpace);
    if (!pointCanMoveTo(sp, pWave.waveIdx, pWave.level))
      return false;
  }
  return true;
}

void Warp::attemptMultiPointDrag(const wave_point_t& newPoint) {
  const float dLevel = newPoint.level - lastDragUpdatePoint.level;
  const int dIndex = newPoint.waveIdx - lastDragUpdatePoint.waveIdx;
  for (auto* sp : selectedPoints) {
    sp->level += dLevel;
    sp->waveIdx += dIndex;
  }
  waveTouched = true;
}

void Warp::attemptDragUpdate(const frect_t& bounds, const fpoint_t& newPoint) {
  if (dragUpdateAllowed(bounds, newPoint)) {
    auto destWP = projectSpaceToWavePoint(bounds, newPoint);
    if (selectedPoints.size() < 2) {  // we're only dragging one point
      movePointConstrained(selectedPoints[0], destWP.waveIdx, destWP.level);
    } else {
      attemptMultiPointDrag(destWP);
    }
    lastDragUpdatePoint = destWP;
  }
}

void Warp::processMouseDown(const frect_t& bounds, const juce::MouseEvent& me) {
  // 1. update the shared state vars
  mouseIsDown = true;
  lastMouseDownPoint = projectSpaceToWavePoint(bounds, me.position);
  lastDragUpdatePoint = lastMouseDownPoint;

  // 2. determine if we clicked on an existing point
  auto clickedPoint = getNearbyPoint(bounds, me.position);
  if (clickedPoint != nullptr) {
    // deal with command-clicking
    if (me.mods.isCommandDown()) {
      if (isPointSelected(clickedPoint)) {
        deselectPoint(clickedPoint);
      } else {
        selectPoint(clickedPoint);
      }
    } else {
      clearSelection();
      selectPoint(clickedPoint);
    }
    downOnSelection = !selectedPoints.empty();
  } else if (selectedPoints.size() > 0) {
    clearSelection();
    downOnSelection = false;
  }
}

void Warp::processMouseDrag(const frect_t& bounds, const juce::MouseEvent& me) {
  if (downOnSelection) {
    auto newPoint = projectSpaceToWavePoint(bounds, me.position);
    attemptDragUpdate(bounds, me.position);
  } else if (mouseIsDown) {
    shouldDrawLasso = true;
    lastDragUpdatePoint = projectSpaceToWavePoint(bounds, me.position);
  }
}

void Warp::processMouseUp(const frect_t& bounds, const juce::MouseEvent& me) {
  // do one more dragUpdate if we're moving points
  if (downOnSelection) {
    attemptDragUpdate(bounds, me.position);
  }
  mouseIsDown = false;
  downOnSelection = false;
  shouldDrawLasso = false;
}

void Warp::processDoubleClick(const frect_t& bounds,
                              const juce::MouseEvent& me) {
  // in any case we'll clear the selection
  clearSelection();
  auto* existingPoint = getNearbyPoint(bounds, me.position);
  if (existingPoint != nullptr) {
    deletePoint((size_t)indexOf(existingPoint));
  } else {
    auto temp = projectSpaceToWavePoint(bounds, me.position);
    createPoint(temp.waveIdx, temp.level);
  }
}

// Drawing-----------------------------------------------------------------------------------

void Warp::drawSection(juce::Graphics& g,
                       const frect_t& bounds,
                       float nStart,
                       float nEnd) {
  if (waveTouched || !fequal(nStart, prevStartNorm) ||
      !fequal(nEnd, prevEndNorm) || true) {
    drawAllParts(g, bounds, nStart, nEnd);
    waveTouched = false;
    prevStartNorm = nStart;
    prevEndNorm = nEnd;
  }
}

void Warp::drawAllParts(juce::Graphics& g,
                        const frect_t& bounds,
                        float nStart,
                        float nEnd) const {
  drawBackground(g, bounds, nStart, nEnd);
  drawWave(g, bounds, nStart, nEnd);
  drawPointHandles(g, bounds, nStart, nEnd);
  if (shouldDrawLasso) {
    auto c1 = projectWavePointToSpace(bounds, lastMouseDownPoint);
    auto c2 = projectWavePointToSpace(bounds, lastDragUpdatePoint);
    frect_t lassoArea(c1, c2);
    drawLasso(g, lassoArea);
  }
}

static color_t bkgndUpper = UIColor::windowBkgnd;
static color_t bkgndLower = UIColor::windowBkgnd.brighter();

void Warp::drawBackground(juce::Graphics& g,
                          const frect_t& bounds,
                          float nStart,
                          float nEnd) const {
  const float x = bounds.getX() + (nStart * bounds.getWidth());
  const float xRight = bounds.getX() + (nEnd * bounds.getWidth());
  frect_t areaUpper = {x, bounds.getY(), xRight - x, bounds.getHeight()};
  frect_t areaLower = areaUpper.removeFromBottom(areaUpper.getHeight() / 2.0f);
  g.setColour(bkgndUpper);
  g.fillRect(areaUpper);
  g.setColour(bkgndLower);
  g.fillRect(areaLower);
}

void Warp::drawWave(juce::Graphics& g,
                    const frect_t& bounds,
                    float nStart,
                    float nEnd) const {
  const int waveStart = (int)(nStart * (float)(TABLE_SIZE - 1));
  const int waveEnd = (int)(nEnd * (float)(TABLE_SIZE - 1));
  const size_t firstPointIdx = leftNeighborPointIndex(waveStart);
  const size_t lastPointIdx = leftNeighborPointIndex(waveEnd) + 1;

  juce::Path pWave;
  pWave.startNewSubPath(bounds.getX() + (bounds.getWidth() * nStart),
                        bounds.getBottom());
  for (size_t i = firstPointIdx; i <= lastPointIdx; ++i) {
    pWave.lineTo(projectWavePointToSpace(bounds, points[i]));
  }
  pWave.lineTo(bounds.getX() + (bounds.getWidth() * nEnd), bounds.getBottom());
  pWave.closeSubPath();

  g.setColour(Color::qualifierPurple.withAlpha(0.8f));
  g.fillPath(pWave);
}

static void s_drawPointHandle(juce::Graphics& g,
                              const fpoint_t& centerPoint,
                              bool isSelected) {
  static const float pointWidth = 12.0f;
  frect_t bounds;
  bounds.setCentre(centerPoint);
  bounds = bounds.withSizeKeepingCentre(pointWidth, pointWidth);
  auto color = isSelected
                   ? Color::qualifierPurple
                   : Color::qualifierPurple.withMultipliedSaturation(0.75f);
  g.setColour(color);
  g.fillEllipse(bounds);
}

void Warp::drawPointHandles(juce::Graphics& g,
                            const frect_t& bounds,
                            float nStart,
                            float nEnd) const {
  const int waveStart = (int)(nStart * (float)(TABLE_SIZE - 1));
  const int waveEnd = (int)(nEnd * (float)(TABLE_SIZE - 1));
  const size_t firstPointIdx = leftNeighborPointIndex(waveStart);
  const size_t lastPointIdx = leftNeighborPointIndex(waveEnd) + 1;
  for (size_t i = firstPointIdx; i <= lastPointIdx; ++i) {
    auto& p = points[i];
    auto center = projectWavePointToSpace(bounds, p);
    s_drawPointHandle(g, center, isPointSelected(&p));
  }
}

void Warp::drawLasso(juce::Graphics& g, const frect_t& lassoArea) const {
  static color_t lassoOutline = UIColor::outline.withAlpha(0.65f);
  static color_t lassoFill = Color::commentGray.withAlpha(0.45f);
  g.setColour(lassoFill);
  g.fillRect(lassoArea);
  g.setColour(lassoOutline);
  g.drawRect(lassoArea, 1.8f);
}

}  // namespace Pointwise
