#include "Electrum/GUI/LFOEditor/LFOEdit.h"
#include "Electrum/Audio/Modulator/LFO.h"
#include "Electrum/GUI/LookAndFeel/Color.h"
#include "Electrum/GUI/LookAndFeel/Fonts.h"
#include "Electrum/GUI/Util/ModalParent.h"
#include "Electrum/Identifiers.h"
#include "Electrum/Shared/ElectrumState.h"
#include "juce_core/juce_core.h"
#include "juce_gui_basics/juce_gui_basics.h"
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
    jassert(lvl <= 1.0f);
    vec.push_back({_t, lvl, locked});
  }
  std::sort(vec.begin(), vec.end(), compareEditHandles);
  return vec;
}

std::vector<edit_handle_t> edit_handle_t::parseShapeString(const String& str) {
  std::vector<lfo_handle_t> chudHandles = {};
  LFO::stringDecode(str, chudHandles);
  jassert(chudHandles.size() > 1);
  std::vector<edit_handle_t> handles = {};
  for (auto& ch : chudHandles) {
    handles.push_back({(int)ch.tableIdx, ch.level, false});
  }
  return handles;
}
//===================================================

LFOEditState::LFOEditState(ElectrumState* s, int idx)
    : handles(edit_handle_t::parseValueTree(LFOID::getLFOEditorTree(s, idx))),
      needsRedraw(true) {
  DBG("Parsed " + String(handles.size()) + " edit handles");
}

LFOEditState::LFOEditState(const String& str)
    : handles(edit_handle_t::parseShapeString(str)) {
  DBG("Parsed " + String(handles.size()) + " edit handles");
  needsRedraw = true;
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

String LFOEditState::encodeCurrentShapeString() const {
  // 1. convert our handles into lfo_handle_t
  std::vector<lfo_handle_t> lHandles = {};
  for (auto& h : handles) {
    lHandles.push_back({h.tableIdx, h.level});
  }
  // 2. parse that list into a string
  return LFO::stringEncode(lHandles);
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
    needsRedraw = true;
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
      needsRedraw = true;
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
    needsRedraw = true;
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
  needsRedraw = true;
}

void LFOEditState::deselectHandle(edit_handle_t* hand) {
  if (lastSelectedHandle == hand) {
    lastSelectedHandle = nullptr;
  }
  auto it = std::next(handles.begin(), indexOf(hand));
  handles.erase(it);
  needsRedraw = true;
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
  needsRedraw = true;
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
  const float levelDelta = -dYNorm;
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
  const float levelDelta = -dYNorm;
  for (auto* s : selectedHandles) {
    auto newTIdx = s->tableIdx + tableIdxDelta;
    auto newLvl = s->level + levelDelta;
    s->tableIdx = newTIdx;
    s->level = newLvl;
  }
  needsRedraw = true;
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
  if (nearbyHandle != nullptr &&
      nearbyHandle->tableIdx > 0) {  // a legal handle was clicked
    // 1. first, handle the command modifier cases
    if (me.mods.isCommandDown()) {
      // just toggle the selection of the clicked handle
      if (isSelected(nearbyHandle)) {
        deselectHandle(nearbyHandle);
      } else {
        selectHandle(nearbyHandle);
        isDraggingSelection = true;
      }
    } else {  // 2. deal with normal clicks
      // if the point we clicked wasn't already selected,
      // deselect the others and select it
      if (!isSelected(nearbyHandle)) {
        clearSelection();
        selectHandle(nearbyHandle);
        isDraggingSelection = true;
      } else {
        // otherwise, we're dragging an existing selection
        isDraggingSelection = true;
      }
    }
  } else if (selectedHandles.size() > 0) {  // no legal handle was clicked
    clearSelection();
    needsRedraw = true;
  }
}

void LFOEditState::dragSinglePoint(const frect_t& bounds,
                                   const fpoint_t& point) {
  auto* pt = selectedHandles[0];
  auto newPtHandle = projectPointToHandle(bounds, point);
  const int newTIdx = (int)newPtHandle.tableIdx;
  const float newLvl = newPtHandle.level;
  if (handleCanMoveTo(pt, newTIdx) && !(newLvl < 0.0f || newLvl > 1.0f)) {
    pt->tableIdx = newTIdx;
    pt->level = newLvl;
    needsRedraw = true;
  }
}

void LFOEditState::processMouseDrag(const frect_t& bounds,
                                    const juce::MouseEvent& me) {
  // two situations where we have to do something:
  // 1. we're dragging a selection, we have to validate
  // and move all the selected handles
  // 2. we're drawing a lasso, we need to update the selection with
  // the handles that are inside of the lasso
  auto mouseHandlePos = projectPointToHandle(bounds, me.position);
  if (isDraggingSelection) {
    if (selectedHandles.size() > 1) {
      // 1. convert the click coords into a lfo_handle_t
      // 2. find our start& end points
      auto lastUpdatePt = projectHandleToPoint(bounds, lastDragUpdateHandlePos);
      // 3. if legal, move the selection and update `lastDragUpdateHandlePos`
      if (dragMovementIsLegal(bounds, lastUpdatePt, me.position)) {
        dragCurrentSelection(bounds, lastUpdatePt, me.position);
      }
    } else {
      jassert(!selectedHandles.empty());
      dragSinglePoint(bounds, me.position);
    }
  } else if (mouseIsDown) {
    shouldDrawLasso = true;
    auto startPt = projectHandleToPoint(bounds, lastMouseDownHandlePos);
    loadLassoIntoSelection(bounds, me.position, startPt);
  }
  lastDragUpdateHandlePos = mouseHandlePos;
}

void LFOEditState::processMouseUp(const frect_t& bounds,
                                  const juce::MouseEvent& me) {
  // 1. if we've been dragging, update the drag selection one more time
  if (isDraggingSelection) {
    if (selectedHandles.size() > 1) {
      // 1. convert the click coords into a lfo_handle_t
      auto mouseHandlePos = projectPointToHandle(bounds, me.position);
      // 2. find our start& end points
      auto lastUpdatePt = projectHandleToPoint(bounds, lastDragUpdateHandlePos);
      // 3. if legal, move the selection and update `lastDragUpdateHandlePos`
      if (dragMovementIsLegal(bounds, lastUpdatePt, me.position)) {
        dragCurrentSelection(bounds, lastUpdatePt, me.position);
        lastDragUpdateHandlePos = mouseHandlePos;
      }
    } else {
      jassert(!selectedHandles.empty());
      dragSinglePoint(bounds, me.position);
      clearSelection();
    }
  }
  // 2. set state variables
  mouseIsDown = false;
  isDraggingSelection = false;
  shouldDrawLasso = false;
  needsRedraw = true;
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
  const int tableIdxStart = (int)(xStart * (float)(LFO_SIZE - 1));
  for (size_t i = 0; i < handles.size() - 1; ++i) {
    if (handles[i].tableIdx <= tableIdxStart &&
        handles[i + 1].tableIdx >= tableIdxStart) {
      return (int)i;
    }
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
  return (int)handles.size() - 1;
}

bool LFOEditState::shouldRedraw(float normStart, float normEnd) const {
  return needsRedraw || !fequal(normStart, lastNormStart) ||
         !fequal(normEnd, lastNormEnd);
}

void LFOEditState::drawAsNeeded(juce::Graphics& g,
                                const frect_t& bounds,
                                float nStart,
                                float nEnd) {
  if (shouldRedraw(nStart, nEnd)) {
    drawSection(g, bounds, nStart, nEnd);
    lastNormStart = nStart;
    lastNormEnd = nEnd;
    needsRedraw = false;
  }
}

void LFOEditState::drawSection(juce::Graphics& g,
                               const frect_t& bounds,
                               float xStart,
                               float xEnd) const {
  drawBackground(g, bounds, xStart, xEnd);
  drawShape(g, bounds, xStart, xEnd);
  drawHandles(g, bounds, xStart, xEnd);
  if (shouldDrawLasso) {
    auto p1 = projectHandleToPoint(bounds, lastMouseDownHandlePos);
    auto p2 = projectHandleToPoint(bounds, lastDragUpdateHandlePos);
    frect_t lassoBox(p1, p2);
    drawLasso(g, lassoBox);
  }
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
  jassert(endHIdx > startHIdx);
  juce::Path p;
  p.startNewSubPath(projectHandleToPoint(bounds, handles[(size_t)startHIdx]));
  for (int i = startHIdx + 1; i <= endHIdx; ++i) {
    p.lineTo(projectHandleToPoint(bounds, handles[(size_t)i]));
  }
  juce::PathStrokeType pst(2.0f);
  g.setColour(Color::literalOrangeBright);
  g.strokePath(p, pst);
}

static void s_drawHandle(juce::Graphics& g,
                         const fpoint_t& centerPt,
                         bool isSelected) {
  const float handleWidth = 13.0f;
  frect_t hBounds;
  hBounds.setCentre(centerPt);
  hBounds = hBounds.withSizeKeepingCentre(handleWidth, handleWidth);
  auto color = isSelected ? Color::qualifierPurple : Color::literalOrangeBright;
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

void LFOEditState::drawLasso(juce::Graphics& g,
                             const frect_t& lassoLimits) const {
  auto strokeColor = Color::commentGray.withAlpha(0.75f);
  auto fillColor = Color::commentGray.withAlpha(0.3f);
  g.setColour(strokeColor);
  g.drawRect(lassoLimits, 1.0f);
  g.setColour(fillColor);
  g.fillRect(lassoLimits);
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
  lfo_handle_t lHand = {handle.tableIdx, handle.level};
  return projectHandleToPoint(bounds, lHand);
}

lfo_handle_t LFOEditState::projectPointToHandle(const frect_t& bounds,
                                                const fpoint_t& point) {
  const float xNorm = (point.getX() - bounds.getX()) / bounds.getWidth();
  const float lvl = (bounds.getBottom() - point.y) / bounds.getHeight();
  auto tableIdx = (int)(xNorm * (float)(LFO_SIZE - 1));
  return {tableIdx, lvl};
}

//============================================================
//--------------COMPONENTS------------------------------------
//============================================================

ViewedLFOEditor::ViewedLFOEditor(ElectrumState* s, int idx)
    : eState(std::make_unique<LFOEditState>(s, idx)) {
  startTimerHz(24);
}

void ViewedLFOEditor::timerCallback() {
  triggerAsyncUpdate();
}

void ViewedLFOEditor::handleAsyncUpdate() {
  repaint();
  auto now = juce::Time::getMillisecondCounter();
  if (now - lastShapeUpdateMs > 850) {
    checkShapeUpdate();
    lastShapeUpdateMs = now;
  }
}

void ViewedLFOEditor::checkShapeUpdate() {
  auto* parent = findParentComponentOfClass<LFOEditor>();
  if (parent != nullptr) {
    if (parent->wantsPreviews()) {
      parent->pushShapeString(getCurrentShapeString());
    }
  }
}

void ViewedLFOEditor::resized() {
  auto* vpt = findParentComponentOfClass<juce::Viewport>();
  if (vpt != nullptr) {
    int width = getLocalBounds().getWidth();
    const float height = (float)vpt->getMaximumVisibleHeight();
    const int viewWidth = vpt->getMaximumVisibleWidth();
    // make sure we can't zoom out so far that part of the viewport is empty
    if (viewWidth > width)
      width = viewWidth;
    setSize(width, (int)(height * 0.9f));
  }
}

void ViewedLFOEditor::setWidthForScale(float normScale) {
  static frange_t scaleRange = rangeWithCenter(0.3f, 5.0f, 1.0f);
  const float fScale = scaleRange.convertFrom0to1(normScale);
  auto* vpt = findParentComponentOfClass<juce::Viewport>();
  if (vpt != nullptr) {
    int width = (int)(fScale * (float)LFO_SIZE);
    const float height = (float)vpt->getMaximumVisibleHeight();
    const int viewWidth = vpt->getMaximumVisibleWidth();
    // make sure we can't zoom out so far that part of the viewport is empty
    if (viewWidth > width)
      width = viewWidth;
    setSize(width, (int)(height * 0.9f));
    eState->requestRedraw();
  }
}

void ViewedLFOEditor::paint(juce::Graphics& g) {
  // 1. grip the parent viewport and find the normalized start/end points
  auto fBounds = getLocalBounds().toFloat();
  auto* vpt = findParentComponentOfClass<juce::Viewport>();
  jassert(vpt != nullptr);
  auto viewedBounds = vpt->getViewArea().toFloat();
  const float normStart = viewedBounds.getX() / fBounds.getWidth();
  const float normEnd = viewedBounds.getRight() / fBounds.getWidth();
  // 2. have the `LFOEditState` do the drawing
  eState->drawSection(g, fBounds, normStart, normEnd);
}

//============================================================
BasicShapeMenu::BasicShapeMenu() {
  // 1. set up the Labels
  headerLabel.aString.setText("Basic LFO Shapes");
  headerLabel.aString.setJustification(juce::Justification::centred);
  headerLabel.aString.setFont(
      FontData::getFontWithHeight(FontE::RobotoMI, 18.0f));
  headerLabel.aString.setColour(UIColor::defaultText);

  shapeLabel.aString.setText("Shape:");
  shapeLabel.aString.setJustification(juce::Justification::centredLeft);
  shapeLabel.aString.setFont(
      FontData::getFontWithHeight(FontE::RobotoMI, 14.0f));
  shapeLabel.aString.setColour(UIColor::defaultText);

  pointsLabel.aString.setText("Points:");
  pointsLabel.aString.setJustification(juce::Justification::centredLeft);
  pointsLabel.aString.setFont(
      FontData::getFontWithHeight(FontE::RobotoMI, 14.0f));
  pointsLabel.aString.setColour(UIColor::defaultText);

  // 2. set up the comboBox
  shapeBox.addItemList(BasicShapeNames, 1);
  shapeBox.setSelectedItemIndex(0);
  addAndMakeVisible(shapeBox);
  // lambda keeps the slider's range appropriate for the
  // shape
  shapeBox.onChange = [this]() {
    BasicShapeE id = (BasicShapeE)shapeBox.getSelectedItemIndex();
    updateSliderRange(id);
  };

  // 3. set up the slider
  pointsSlider.setSliderStyle(juce::Slider::LinearHorizontal);
  pointsSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 30, 12);
  addAndMakeVisible(pointsSlider);
  updateSliderRange(BasicShapeE::Sine);

  // 4. set up the OK button
  okButton.setButtonText("Ok");
  addAndMakeVisible(okButton);
  okButton.onClick = [this]() { loadSelectionToEditor(); };
}

void BasicShapeMenu::updateSliderRange(BasicShapeE id) {
  juce::Range<double> range;
  double defaultVal;
  switch (id) {
    case Sine:
      range = juce::Range<double>(5.0, 100.0);
      defaultVal = 35.0;
      break;
    case RisingRamp:
      range = juce::Range<double>(2.0, 100.0);
      defaultVal = 2.0;
      break;
    case FallingRamp:
      range = juce::Range<double>(2.0, 100.0);
      defaultVal = 2.0;
      break;
    case Triangle:
      range = juce::Range<double>(2.0, 100.0);
      defaultVal = 2.0;
      break;
    case Random:
      range = juce::Range<double>(3.0, 100.0);
      defaultVal = 8.0;
      break;
  }
  pointsSlider.setRange(range, 1.0);
  pointsSlider.setValue(defaultVal);
}

static handle_vector_t s_getSineHandles(int numPoints) {
  handle_vector_t handles = {};
  for (int i = 0; i < numPoints; ++i) {
    float normPhase = (float)i / (float)(numPoints - 1);
    const float angle = normPhase * juce::MathConstants<float>::twoPi;
    const float lvl = (std::sinf(angle) + 1.0f) * 0.5f;
    const int tIdx = (int)(normPhase * (float)(LFO_SIZE - 1));
    handles.push_back({tIdx, lvl});
  }
  return handles;
}

static handle_vector_t s_getRisingHandles(int numPoints) {
  handle_vector_t handles = {};
  for (int i = 0; i < numPoints; ++i) {
    float normPhase = (float)i / (float)(numPoints - 1);
    const int tIdx = (int)(normPhase * (float)(LFO_SIZE - 1));
    handles.push_back({tIdx, normPhase});
  }
  return handles;
}

static handle_vector_t s_getFallingHandles(int numPoints) {
  handle_vector_t handles = {};
  for (int i = 0; i < numPoints; ++i) {
    float normPhase = (float)i / (float)(numPoints - 1);
    const int tIdx = (int)(normPhase * (float)(LFO_SIZE - 1));
    handles.push_back({tIdx, 1.0f - normPhase});
  }
  return handles;
}

static handle_vector_t s_getTriangleHandles(int numPoints) {
  handle_vector_t handles = {};
  for (int i = 0; i < numPoints; ++i) {
    float normPhase = (float)i / (float)(numPoints - 1);
    const float distFromPeak = std::fabs(0.5f - normPhase);
    const int tIdx = (int)(normPhase * (float)(LFO_SIZE - 1));
    handles.push_back({tIdx, 1.0f - (2.0f * distFromPeak)});
  }
  return handles;
}

static handle_vector_t s_getRandomHandles(int numPoints) {
  juce::Random rng;
  handle_vector_t handles = {};
  for (int i = 0; i < numPoints; ++i) {
    float normPhase = (float)i / (float)(numPoints - 1);
    const int tIdx = (int)(normPhase * (float)(LFO_SIZE - 1));
    handles.push_back({tIdx, rng.nextFloat()});
  }
  return handles;
}

handle_vector_t BasicShapeMenu::getHandlesForSelection() {
  const int numPoints = (int)pointsSlider.getValue();
  BasicShapeE id = (BasicShapeE)shapeBox.getSelectedItemIndex();
  switch (id) {
    case Sine:
      return s_getSineHandles(numPoints);
    case RisingRamp:
      return s_getRisingHandles(numPoints);
    case FallingRamp:
      return s_getFallingHandles(numPoints);
    case Triangle:
      return s_getTriangleHandles(numPoints);
    case Random:
      return s_getRandomHandles(numPoints);
  }
  jassert(false);
  return s_getSineHandles(numPoints);
}

void BasicShapeMenu::loadSelectionToEditor() {
  auto editor = findParentComponentOfClass<LFOEditor>();
  if (editor != nullptr) {
    auto handles = getHandlesForSelection();
    const String shapeStr = LFO::stringEncode(handles);
    editor->replaceEditorShapeString(shapeStr);
  } else {
    jassert(false);
  }
}

void BasicShapeMenu::resized() {
  auto fBounds = getLocalBounds().toFloat().reduced(5.0f);
  headerLabel.bounds = fBounds.removeFromTop(30.0f);
  shapeLabel.bounds = fBounds.removeFromTop(16.0f);

  auto shapeArea = fBounds.removeFromTop(30.0f).reduced(2.0f);
  shapeBox.setBounds(shapeArea.toNearestInt());

  pointsLabel.bounds = fBounds.removeFromTop(16.0f);

  auto ptsArea = fBounds.removeFromTop(45.0f).reduced(2.0f);
  pointsSlider.setBounds(ptsArea.toNearestInt());

  auto okBounds = fBounds.removeFromTop(30.0f).reduced(2.0f);
  okButton.setBounds(okBounds.toNearestInt());
}

void BasicShapeMenu::paint(juce::Graphics& g) {
  auto fBounds = getLocalBounds().toFloat();
  g.setColour(UIColor::menuBkgnd);
  g.fillRect(fBounds);
  headerLabel.draw(g);
  shapeLabel.draw(g);
  pointsLabel.draw(g);
}

//============================================================

LFOEditor::PreviewBtn::PreviewBtn() : juce::Button("PreviewBtn") {
  aStr.setFont(FontData::getFontWithHeight(FontE::RobotoMI, 14.0f));
  aStr.setJustification(juce::Justification::centred);
  aStr.setText("Preview");
  setClickingTogglesState(true);
}

void LFOEditor::PreviewBtn::paintButton(juce::Graphics& g, bool, bool) {
  auto fBounds = getLocalBounds().toFloat();
  static const float corner = 3.0f;
  g.setColour(UIColor::widgetBkgnd);
  g.fillRoundedRectangle(fBounds, corner);
  auto fillColor =
      getToggleState() ? Color::assignmentPink : Color::commentGray;
  g.setColour(fillColor);
  g.drawRoundedRectangle(fBounds, corner, 1.8f);
  aStr.setColour(fillColor);
  auto textBounds = fBounds.reduced(2.0f);
  aStr.draw(g, textBounds);
}

LFOEditor::LFOEditor(ElectrumState* s, int idx)
    : state(s), lfoID(idx), editor(s, idx) {
  // grip the saved shape string
  savedShapeString = editor.getCurrentShapeString();
  // 1. set up the view port
  vpt.setViewedComponent(&editor, false);
  vpt.setViewPosition(0, 0);
  vpt.setInterceptsMouseClicks(true, true);
  vpt.setRepaintsOnMouseActivity(false);
  addAndMakeVisible(vpt);
  editor.resized();
  // 2. set up the zoom slider
  zoomSlider.setSliderStyle(juce::Slider::LinearVertical);
  zoomSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 1, 1);
  zoomSlider.setRange(0.0, 1.0);
  addAndMakeVisible(zoomSlider);
  zoomSlider.onValueChange = [this]() {
    const float zNorm = (float)zoomSlider.getValue();
    editor.setWidthForScale(zNorm);
  };
  // 4. set up the label strings
  zoomStr.aString.setText("Zoom");
  zoomStr.aString.setJustification(juce::Justification::centred);
  zoomStr.aString.setFont(FontData::getFontWithHeight(FontE::RobotoMI, 14.0f));
  zoomStr.aString.setColour(UIColor::defaultText);
  // 5. set up the save and close buttons
  saveButton.setButtonText("Save");
  addAndMakeVisible(saveButton);
  saveButton.onClick = [this]() {
    pushShapeString(editor.getCurrentShapeString());
    ModalParent::exitModalView(this);
  };
  closeButton.setButtonText("Close");
  addAndMakeVisible(closeButton);
  closeButton.onClick = [this]() {
    if (previewEditorShape) {
      pushShapeString(savedShapeString);
    }
    ModalParent::exitModalView(this);
  };
  zoomSlider.setValue(0.015);
  // and the "preview button"
  savedShapeString = editor.getCurrentShapeString();
  addAndMakeVisible(previewBtn);
  // assign the callback
  previewBtn.onClick = [this] { previewBtnClicked(); };
  addAndMakeVisible(basicMenu);
}

LFOEditor::~LFOEditor() {
  auto parent = getParentComponent();
  if (parent != nullptr) {
    parent->removeChildComponent(this);
  }
}

void LFOEditor::resized() {
  auto fBounds = getLocalBounds().toFloat().reduced(12.0f);

  // 1. bottom buttons
  auto btnArea = fBounds.removeFromBottom(30.0f);
  auto closeArea =
      btnArea.removeFromLeft(btnArea.getWidth() / 3.0f).reduced(3.0f);
  auto saveArea =
      btnArea.removeFromLeft(btnArea.getWidth() / 3.0f).reduced(3.0f);
  auto previewArea = btnArea.reduced(3.0f);
  saveButton.setBounds(saveArea.toNearestInt());
  closeButton.setBounds(closeArea.toNearestInt());
  previewBtn.setBounds(previewArea.toNearestInt());
  // 2. zoom slider
  auto zArea = fBounds.removeFromLeft(40.0f);
  zoomStr.bounds = zArea.removeFromTop(16.0f);
  auto sliderArea = zArea.reduced(2.5f);
  zoomSlider.setBounds(sliderArea.toNearestInt());
  // 3. basic menu
  auto menuArea = fBounds.removeFromRight(140.0f);
  basicMenu.setBounds(menuArea.toNearestInt());
  // 4. editor
  auto vptBounds = fBounds.reduced(2.0f);
  vpt.setBounds(vptBounds.toNearestInt());
  vpt.toFront(true);
}

void LFOEditor::paint(juce::Graphics& g) {
  auto fBounds = getLocalBounds().toFloat();
  g.setColour(Color::assignmentPink);
  g.fillRect(fBounds);
  fBounds = fBounds.reduced(2.5f);
  g.setColour(UIColor::menuBkgnd);
  g.fillRect(fBounds);
  zoomStr.draw(g);
}

void LFOEditor::previewBtnClicked() {
  auto newState = previewBtn.getToggleState();
  if (newState != previewEditorShape) {
    previewEditorShape = newState;
    if (previewEditorShape) {
      auto newShape = editor.getCurrentShapeString();
      pushShapeString(newShape);
    } else {
      pushShapeString(savedShapeString);
    }
  }
}

void LFOEditor::pushShapeString(const String& str) {
  // sinple
  state->updateLFOString(str, lfoID);
}
