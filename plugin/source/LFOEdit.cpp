#include "Electrum/GUI/LFOEditor/LFOEdit.h"
#include "Electrum/Audio/Modulator/LFO.h"
#include "Electrum/GUI/LookAndFeel/Color.h"
#include "Electrum/Identifiers.h"
namespace LFOID {

ValueTree getLFOEditorTree(ElectrumState* s, int idx) {
  auto infoTree = s->state.getChildWithName(ID::LFO_INFO);
  const String propID = ID::lfoShapeString.toString() + String(idx);
  String fullStr = infoTree[propID];
  std::vector<lfo_handle_t> handles = {};
  LFO::stringDecode(fullStr, handles);
  ValueTree vt(LFO_EDIT_STATE);
  vt.setProperty(lfoIndex, idx, nullptr);
  for (auto& h : handles) {
    ValueTree child(LFO_HANDLE_STATE);
    child.setProperty(handleLevel, h.level, nullptr);
    child.setProperty(handleTableIndex, (int)h.tableIdx, nullptr);
    child.setProperty(handleIsLocked, false, nullptr);
    vt.appendChild(child, nullptr);
  }
  return vt;
}
}  // namespace LFOID

static bool compareEditHandles(edit_handle_t a, edit_handle_t b) {
  return a.tableIdx < b.tableIdx;
}

static bool compareEditPtrs(edit_handle_t* a, edit_handle_t* b) {
  return a->tableIdx < b->tableIdx;
}
//---------------------------------------------------
std::vector<edit_handle_t> edit_handle_t::parseValueTree(
    const ValueTree& tree) {
  std::vector<edit_handle_t> vec = {};
  jassert(tree.hasType(LFOID::LFO_EDIT_STATE));
  for (auto it = tree.begin(); it != tree.end(); ++it) {
    auto hTree = *it;
    const int _t = hTree[LFOID::handleTableIndex];
    const float lvl = hTree[LFOID::handleLevel];
    const bool locked = hTree[LFOID::handleIsLocked];
    vec.push_back({_t, lvl, locked});
  }
  std::sort(vec.begin(), vec.end(), compareEditHandles);
  return vec;
}
//===================================================

LFOEditState::LFOEditState(ElectrumState* s, int idx)
    : editState(LFOID::getLFOEditorTree(s, idx)),
      handles(edit_handle_t::parseValueTree(editState)),
      lfoID(idx) {
  DBG("Parsed " + String(handles.size()) + " edit handles");
}

bool LFOEditState::handleExistsAt(int tableIdx) const {
  for (auto& h : handles) {
    if (h.tableIdx == tableIdx)
      return true;
  }
  return false;
}

int LFOEditState::handleIndexAt(int tableIndex) const {
  for (size_t i = 0; i < handles.size(); ++i) {
    auto& h = handles[i];
    if (h.tableIdx == tableIndex)
      return (int)i;
  }
  return -1;
}

bool LFOEditState::handleCanMoveTo(int handleIndex, int newTableIndex) const {
  if (handleIndex < 1) {
    return newTableIndex == 0;
  }
  auto& h = handles[(size_t)handleIndex];
  if (h.locked) {
    return false;
  }
  // 1. determine which direction we're asking to move
  if (newTableIndex < h.tableIdx) {
    return handles[(size_t)handleIndex - 1].tableIdx < newTableIndex;
  } else {
    if (handleIndex >= (int)handles.size() - 1) {
      return newTableIndex < LFO_SIZE;
    } else {
      return newTableIndex < handles[(size_t)handleIndex + 1].tableIdx;
    }
  }
}

bool LFOEditState::handleIsLocked(int handleIndex) const {
  return handleIndex < 1 || handles[(size_t)handleIndex].locked;
}

bool LFOEditState::handleCanMoveTo(edit_handle_t* ptr, int tableIdx) const {
  auto idx = indexOf(ptr);
  return handleCanMoveTo(idx, tableIdx);
}

//----------------------------------------------

int LFOEditState::handleIdxToLeft(int tableIdx) const {
  for (size_t i = 1; i < handles.size(); ++i) {
    auto& hLeft = handles[i - 1];
    auto& hRight = handles[i];
    if (hLeft.tableIdx < tableIdx && hRight.tableIdx >= tableIdx) {
      return (int)(i - 1);
    }
  }
  return -1;
}

bool LFOEditState::canCreateHandle(int tableIdx, float lvl) const {
  if (handleExistsAt(tableIdx))
    return false;
  if (tableIdx < 1 || tableIdx >= LFO_SIZE)
    return false;
  return lvl >= 0.0f && lvl < 1.0f;
}

int LFOEditState::indexOf(edit_handle_t* handle) const {
  for (size_t i = 0; i < handles.size(); ++i) {
    if (handle == &handles[i])
      return (int)i;
  }
  jassert(false);
  return -1;
}

int LFOEditState::createHandle(int tableIdx, float level) {
  if (canCreateHandle(tableIdx, level)) {
    // 1. find the neighbor
    const int leftHandle = handleIdxToLeft(tableIdx);
    jassert(leftHandle != -1);
    auto& leftObj = handles[(size_t)leftHandle];
    jassert(leftObj.tableIdx < tableIdx);
    // 2. create the object
    edit_handle_t eh;
    eh.tableIdx = tableIdx;
    eh.level = level;
    eh.locked = false;

    // 3. add it to the vector and sort
    handles.push_back(eh);
    std::sort(handles.begin(), handles.end(), compareEditHandles);
    return leftHandle + 1;
  }
  return -1;
}

bool LFOEditState::removeHandle(int handleIdx) {
  if (handleIdx > 0 && handleIdx < (int)handles.size()) {
    if (!handleIsLocked(handleIdx) && handles.size() > 2) {
      // 1. remove from the vector
      auto removeIt = std::next(handles.begin(), handleIdx);
      handles.erase(removeIt);
    }
    return true;
  }
  return false;
}

// Mouse Handling stuff=======================================
edit_handle_t* LFOEditState::getHandleWithinDist(const frect_t& bounds,
                                                 const fpoint_t& point,
                                                 float thresh) {
  int nearestIdx = findClosestEditHandle(bounds, point);
  if (nearestIdx < 1)
    return nullptr;
  auto& handle = handles[(size_t)nearestIdx];
  auto hPoint = projectHandleToPoint(bounds, handle);
  if (point.getDistanceFrom(hPoint) < thresh) {
    return &handle;
  }
  return nullptr;
}

void LFOEditState::selectHandle(edit_handle_t* hand) {
  if (hand->locked)
    return;
  if (hand != lastSelectedHandle) {
    lastSelectedHandle = hand;
    selectedHandles.push_back(hand);
    std::sort(selectedHandles.begin(), selectedHandles.end(), compareEditPtrs);
  }
}

void LFOEditState::deleteSelectedHandles() {
  // 1. copy the table indices of the currently selected
  // handles into a new vector on da stack
  std::vector<int> selectedTableIndices = {};
  for (auto* s : selectedHandles) {
    selectedTableIndices.push_back(s->tableIdx);
  }
  // 2. empty the `selectedHandles` vector and so on
  selectedTableIndices.clear();
  lastSelectedHandle = nullptr;
  // 3. find and delete each of the handles by index
  for (auto& tIdx : selectedTableIndices) {
    int hIdx = handleIndexAt(tIdx);
    jassert(hIdx > -1);
    jassert(removeHandle(hIdx));
  }
}

void LFOEditState::deselectHandle(edit_handle_t* hand) {
  if (lastSelectedHandle == hand) {
    lastSelectedHandle = nullptr;
  }
  auto it = std::next(handles.begin(), indexOf(hand));
  handles.erase(it);
  if (lastSelectedHandle == nullptr && !handles.empty()) {
    lastSelectedHandle = &handles.back();
  }
}

bool LFOEditState::isSelected(const edit_handle_t* hand) const {
  for (auto* h : selectedHandles) {
    if (h == hand)
      return true;
  }
  return false;
}

void LFOEditState::loadLassoIntoSelection(const frect_t& bounds,
                                          const fpoint_t& cornerA,
                                          const fpoint_t& cornerB) {
  selectedHandles.clear();
  frect_t lassoBounds(cornerA, cornerB);
  for (auto& h : handles) {
    if (!h.locked) {
      auto hPoint = projectHandleToPoint(bounds, h);
      if (lassoBounds.contains(hPoint)) {
        selectedHandles.push_back(&h);
      }
    }
  }
  std::sort(selectedHandles.begin(), selectedHandles.end(), compareEditPtrs);
}

bool LFOEditState::dragMovementIsLegal(const frect_t& bounds,
                                       const fpoint_t& startPt,
                                       const fpoint_t& endPt) const {
  // 1. figure out how much we've moved along each
  // axis in relative (normalized) terms
  const float dXNorm = (endPt.x - startPt.x) / bounds.getWidth();
  const float dYNorm = (endPt.y - startPt.y) / bounds.getHeight();
  // 2. translate that to changes in level/tableIdx
  const int tableIdxDelta = (int)(dXNorm * (float)(LFO_SIZE - 1));
  const float levelDelta = 1.0f - dYNorm;
  // 3. check each selected point
  for (auto* s : selectedHandles) {
    auto newTIdx = s->tableIdx + tableIdxDelta;
    auto newLvl = s->level + levelDelta;
    if (newLvl < 0.0f || newLvl > 1.0f) {  // level out of bounds
      return false;
    }
    if (!handleCanMoveTo(s, newTIdx))
      return false;
  }
  return true;
}

void LFOEditState::dragCurrentSelection(const frect_t& bounds,
                                        const fpoint_t& startPt,
                                        const fpoint_t& endPt) {
  const float dXNorm = (endPt.x - startPt.x) / bounds.getWidth();
  const float dYNorm = (endPt.y - startPt.y) / bounds.getHeight();
  const int tableIdxDelta = (int)(dXNorm * (float)(LFO_SIZE - 1));
  const float levelDelta = 1.0f - dYNorm;
  for (auto* s : selectedHandles) {
    auto newTIdx = s->tableIdx + tableIdxDelta;
    auto newLvl = s->level + levelDelta;
    s->tableIdx = newTIdx;
    s->level = newLvl;
  }
}

void LFOEditState::processMouseDown(const frect_t& bounds,
                                    const juce::MouseEvent& me) {
  isDraggingSelection = false;
  // 1. update these bits of state in any case
  mouseIsDown = true;
  lastMouseDownHandlePos = projectPointToHandle(bounds, me.position);
  lastDragUpdateHandlePos = lastMouseDownHandlePos;
  // 2. if we're close to an existing handle, we may have more to do
  auto nearbyHandle = getHandleWithinDist(bounds, me.position);
  if (nearbyHandle != nullptr && nearbyHandle->tableIdx > 0) {
    if (me.mods.isCommandDown()) {
      if (isSelected(nearbyHandle)) {
        deselectHandle(nearbyHandle);
      } else {
        selectHandle(nearbyHandle);
        isDraggingSelection = true;
      }
    } else if (isSelected(nearbyHandle) && selectedHandles.size() > 1) {
      isDraggingSelection = true;
    } else {
      clearSelection();
      selectHandle(nearbyHandle);
      isDraggingSelection = true;
    }
  } else {  // no legal handle was clicked
    clearSelection();
  }
}

void LFOEditState::processMouseDrag(const frect_t& bounds,
                                    const juce::MouseEvent& me) {
  // two situations where we have to do something:
  // 1. we're dragging a selection, we have to validate
  // and move all the selected handles
  // 2. we're drawing a lasso, we need to update the selection with
  // the handles that are inside of the lasso
  if (isDraggingSelection) {
    // 1. convert the click coords into a lfo_handle_t
    auto mouseHandlePos = projectPointToHandle(bounds, me.position);
    // 2. find our start& end points
    auto lastUpdatePt = projectHandleToPoint(bounds, lastDragUpdateHandlePos);
    // 3. if legal, move the selection and update `lastDragUpdateHandlePos`
    if (dragMovementIsLegal(bounds, lastUpdatePt, me.position)) {
      dragCurrentSelection(bounds, lastUpdatePt, me.position);
      lastDragUpdateHandlePos = mouseHandlePos;
    }
  } else if (mouseIsDown) {
    auto startPt = projectHandleToPoint(bounds, lastMouseDownHandlePos);
    loadLassoIntoSelection(bounds, me.position, startPt);
  }
}

void LFOEditState::processMouseUp(const frect_t& bounds,
                                  const juce::MouseEvent& me) {
  // 1. if we've been dragging, update the drag selection one more time
  if (isDraggingSelection) {
    // 1. convert the click coords into a lfo_handle_t
    auto mouseHandlePos = projectPointToHandle(bounds, me.position);
    // 2. find our start& end points
    auto lastUpdatePt = projectHandleToPoint(bounds, lastDragUpdateHandlePos);
    // 3. if legal, move the selection and update `lastDragUpdateHandlePos`
    if (dragMovementIsLegal(bounds, lastUpdatePt, me.position)) {
      dragCurrentSelection(bounds, lastUpdatePt, me.position);
      lastDragUpdateHandlePos = mouseHandlePos;
    }
  }
  // 2. set state variables
  mouseIsDown = false;
  isDraggingSelection = false;
}

void LFOEditState::processDoubleClick(const frect_t& bounds,
                                      const juce::MouseEvent& me) {
  auto* existing = getHandleWithinDist(bounds, me.position);
  if (existing != nullptr && !existing->locked) {
    auto idx = indexOf(existing);
    removeHandle(idx);
  } else {
    // figure out what table index was clicked on
    const float xNorm = (me.position.x - bounds.getX()) / bounds.getWidth();
    const int tIdx = (int)(xNorm * (float)(LFO_SIZE - 1));
    if (!handleExistsAt(tIdx)) {
      const float lvl =
          (bounds.getBottom() - me.position.y) / bounds.getHeight();
      createHandle(tIdx, lvl);
    }
  }
}

//-----------------------------------------------------------
int LFOEditState::findClosestEditHandle(const frect_t& bounds,
                                        const fpoint_t& point) {
  auto h0 = projectPointToHandle(bounds, point);
  const int lIdx = handleIdxToLeft((int)h0.tableIdx);
  const int rIdx = lIdx + 1;
  if (lIdx < (int)handles.size() - 1) {
    auto lPoint = projectHandleToPoint(bounds, handles[(size_t)lIdx]);
    auto rPoint = projectHandleToPoint(bounds, handles[(size_t)rIdx]);
    return (point.getDistanceFrom(lPoint) < point.getDistanceFrom(rPoint))
               ? lIdx
               : rIdx;
  } else {
    return (int)(handles.size() - 1);
  }
}

// Drawing===================================================================

int LFOEditState::firstHandleToDraw(float xStart) const {
  for (size_t i = 0; i < handles.size() - 1; ++i) {
    const float normLow = (float)handles[i].tableIdx / (float)(LFO_SIZE - 1);
    const float normHigh = (float)handles[i].tableIdx / (float)(LFO_SIZE - 1);
    if (normLow <= xStart && normHigh > xStart)
      return (int)i;
  }
  jassert(false);
  return -1;
}

int LFOEditState::lastHandleToDraw(float xEnd) const {
  for (size_t i = 0; i < handles.size(); ++i) {
    // easy bc `handles` is sorted
    const float xNorm = (float)handles[i].tableIdx / (float)(LFO_SIZE - 1);
    if (xNorm >= xEnd)
      return (int)i;
  }
  jassert(false);
  return -1;
}

void LFOEditState::drawSection(juce::Graphics& g,
                               const frect_t& bounds,
                               float xStart,
                               float xEnd) const {
  drawBackground(g, bounds, xStart, xEnd);
  drawShape(g, bounds, xStart, xEnd);
  drawHandles(g, bounds, xStart, xEnd);
}

static frect_t s_getVisibleWindow(const frect_t& bounds,
                                  float xStart,
                                  float xEnd) {
  const float xLeft = bounds.getX() + (xStart * bounds.getWidth());
  const float xRight = bounds.getX() + (xEnd * bounds.getWidth());
  return {xLeft, bounds.getY(), xRight - xLeft, bounds.getHeight()};
}

void LFOEditState::drawBackground(juce::Graphics& g,
                                  const frect_t& bounds,
                                  float normStart,
                                  float normEnd) const {
  auto window = s_getVisibleWindow(bounds, normStart, normEnd);
  g.setColour(Color::nearBlack);
  g.fillRect(window);
}

void LFOEditState::drawShape(juce::Graphics& g,
                             const frect_t& bounds,
                             float normStart,
                             float normEnd) const {
  auto startHIdx = firstHandleToDraw(normStart);
  auto endHIdx = lastHandleToDraw(normEnd);
  juce::Path p;
  p.startNewSubPath(projectHandleToPoint(bounds, handles[(size_t)startHIdx]));
  for (int i = startHIdx + 1; i <= endHIdx; ++i) {
    p.lineTo(projectHandleToPoint(bounds, handles[(size_t)i]));
  }
  juce::PathStrokeType pst(2.0f);
  g.setColour(Color::qualifierPurple);
  g.strokePath(p, pst);
}

static void s_drawHandle(juce::Graphics& g,
                         const fpoint_t& centerPt,
                         bool isSelected) {
  const float handleWidth = 13.0f;
  frect_t hBounds;
  hBounds.setCentre(centerPt);
  hBounds = hBounds.withSizeKeepingCentre(handleWidth, handleWidth);
  auto color =
      isSelected ? Color::qualifierPurple.brighter() : Color::qualifierPurple;
  g.setColour(color);
  g.fillEllipse(hBounds);
}

void LFOEditState::drawHandles(juce::Graphics& g,
                               const frect_t& bounds,
                               float normStart,
                               float normEnd) const {
  auto startHIdx = firstHandleToDraw(normStart);
  auto endHIdx = lastHandleToDraw(normEnd);
  for (auto i = startHIdx; i <= endHIdx; ++i) {
    auto& h = handles[(size_t)i];
    auto center = projectHandleToPoint(bounds, h);
    s_drawHandle(g, center, isSelected(&handles[(size_t)i]));
  }
}

//===========================================================================

// we do a little geometry/projection
fpoint_t LFOEditState::projectHandleToPoint(const frect_t& bounds,
                                            const lfo_handle_t& handle) {
  const float xNorm = (float)handle.tableIdx / (float)(LFO_SIZE - 1);
  return {bounds.getX() + (bounds.getWidth() * xNorm),
          bounds.getBottom() - (bounds.getHeight() * handle.level)};
}

fpoint_t LFOEditState::projectHandleToPoint(const frect_t& bounds,
                                            const edit_handle_t& handle) {
  lfo_handle_t lHand = {(size_t)handle.tableIdx, handle.level};
  return projectHandleToPoint(bounds, lHand);
}

lfo_handle_t LFOEditState::projectPointToHandle(const frect_t& bounds,
                                                const fpoint_t& point) {
  const float xNorm = (point.getX() - bounds.getX()) / bounds.getWidth();
  const float lvl = (bounds.getBottom() - point.y) / bounds.getHeight();
  auto tableIdx = (size_t)(xNorm * (float)(LFO_SIZE - 1));
  return {tableIdx, lvl};
}

