#include "Electrum/Shared/PointwiseWave.h"
#include "Electrum/Audio/AudioUtil.h"
#include "Electrum/Audio/Wavetable.h"

namespace Pointwise {
wave_point_t projectSpaceToWavePoint(const frect_t& bounds,
                                     const fpoint_t& point) {
  const float lvlNorm = (bounds.getBottom() - point.y) / bounds.getHeight();
  const float xNorm = (point.x - bounds.getX()) / bounds.getWidth();
  const int waveIdx = (int)(xNorm * (float)(TABLE_SIZE - 1));
  const float lvl = (lvlNorm * 2.0f) - 1.0f;
  return {waveIdx, lvl, false, 0};
}
fpoint_t projectWavePointToSpace(const frect_t& bounds,
                                 const wave_point_t& point) {
  const float yNorm = (point.level + 1.0f) * 0.5f;
  const float y = bounds.getBottom() - (bounds.getHeight() * yNorm);
  const float xNorm = (float)point.waveIdx / (float)TABLE_SIZE;
  const float x = bounds.getX() + (xNorm * bounds.getWidth());
  return {x, y};
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
  for (int i = 0; i < (int)points.size(); ++i) {
    if (pt == &points[(size_t)i])
      return i;
  }
  jassert(false);
  return -1;
}

static bool s_compareWavePts(wave_point_t a, wave_point_t b) {
  return a.waveIdx < b.waveIdx;
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
  if (points[closest].waveIdx < waveIdx) {
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
  if (pt->xAxisLocked)
    return false;
  auto nLeft = leftNeighborPointIndex(pt->waveIdx);
  auto nRight = nLeft + 1;
  jassert(nRight < points.size());
  return points[nLeft].waveIdx < waveIdx && points[nRight].waveIdx > waveIdx;
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
    auto nLeft = leftNeighborPointIndex(pt->waveIdx);
    auto nRight = nLeft + 1;
    jassert(nRight < points.size());
    const int minWaveIdx = points[nLeft].waveIdx + 1;
    const int maxWaveIdx = points[nRight].waveIdx - 1;
    pt->waveIdx = std::clamp(waveIdx, minWaveIdx, maxWaveIdx);
  }
}

size_t Warp::createPoint(int waveIdx, float lvl, int wType) {
  if (canCreatePoint(waveIdx, lvl)) {
    wave_point_t newPoint = {waveIdx, lvl, false, wType};
    points.push_back(newPoint);
    sortPoints();
  }
  return closestPointIndex(waveIdx);
}

bool Warp::deletePoint(size_t pointIdx) {
  if (pointIdx == 0 || pointIdx == points.size() - 1) {
    return false;
  }
  if (!points[pointIdx].xAxisLocked) {
    auto iDelete = std::next(points.begin(), (int)pointIdx);
    points.erase(iDelete);
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
}

void Warp::clearSelection() {
  lastSelectedPt = nullptr;
  selectedPoints.clear();
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

void Warp::attemptDragUpdate(const wave_point_t& newPoint) {
  if (dragUpdateAllowed(newPoint)) {
    const float dLevel = newPoint.level - lastDragUpdatePoint.level;
    const int dIndex = newPoint.waveIdx - lastDragUpdatePoint.waveIdx;
    for (auto* sp : selectedPoints) {
      sp->level += dLevel;
      sp->waveIdx += dIndex;
    }
    lastDragUpdatePoint = newPoint;
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
    attemptDragUpdate(newPoint);
  } else if (mouseIsDown) {
    shouldDrawLasso = true;
    lastDragUpdatePoint = projectSpaceToWavePoint(bounds, me.position);
  }
}

void Warp::processMouseUp(const frect_t& bounds, const juce::MouseEvent& me) {
  // do one more dragUpdate if we're moving points
  if (downOnSelection) {
    auto newPoint = projectSpaceToWavePoint(bounds, me.position);
    attemptDragUpdate(newPoint);
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

}  // namespace Pointwise
