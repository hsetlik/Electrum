#include "Electrum/GUI/LFOEditor/LFOEdit.h"
#include "Electrum/Audio/Modulator/LFO.h"
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
  int editIdx = 0;
  for (auto& h : handles) {
    ValueTree child(LFO_HANDLE_STATE);
    child.setProperty(handleLevel, h.level, nullptr);
    child.setProperty(handleTableIndex, (int)h.tableIdx, nullptr);
    child.setProperty(handleIndex, editIdx, nullptr);
    child.setProperty(handleIsLocked, false, nullptr);
    vt.appendChild(child, nullptr);
    ++editIdx;
  }
  return vt;
}
}  // namespace LFOID

static bool compareEditHandles(edit_handle_t a, edit_handle_t b) {
  return a.handleIdx < b.handleIdx;
}
//---------------------------------------------------
std::vector<edit_handle_t> edit_handle_t::parseValueTree(
    const ValueTree& tree) {
  std::vector<edit_handle_t> vec = {};
  jassert(tree.hasType(LFOID::LFO_EDIT_STATE));
  for (auto it = tree.begin(); it != tree.end(); ++it) {
    auto hTree = *it;
    const int _t = hTree[LFOID::handleTableIndex];
    const int _h = hTree[LFOID::handleIndex];
    const float lvl = hTree[LFOID::handleLevel];
    const bool locked = hTree[LFOID::handleIsLocked];
    vec.push_back({_t, _h, lvl, locked});
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
  if (handleIndex < 1)
    return (newTableIndex == 0);
  auto& h = handles[(size_t)handleIndex];
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

int LFOEditState::createHandle(int tableIdx, float level) {
  if (canCreateHandle(tableIdx, level)) {
    // 1. find the neighbor
    const int leftHandle = handleIdxToLeft(tableIdx);
    jassert(leftHandle != -1);
    auto& leftObj = handles[(size_t)leftHandle];
    jassert(leftObj.tableIdx < tableIdx);
    // 2. create the object
    edit_handle_t eh;
    eh.handleIdx = leftHandle + 1;
    eh.tableIdx = tableIdx;
    eh.level = level;
    eh.locked = false;
    // 3. insert it and hold on to the iterator
    auto insIterator = std::next(handles.begin(), leftHandle);
    handles.insert(insIterator, eh);
    if (leftHandle < (int)(handles.size() - 1)) {
      // 4. increment the handle idx of everything to the right
      for (auto it = insIterator + 1; it != handles.end(); ++it) {
        auto& obj = *it;
        ++obj.handleIdx;
      }
    }
    return leftHandle + 1;
  }
  return -1;
}

bool LFOEditState::removeHandle(int handleIdx) {
  if (handleIdx > 0 && handleIdx < (int)handles.size()) {
    if (!handleIsLocked(handleIdx)) {
      // 1. remove from the vector
      auto removeIt = std::next(handles.begin(), handleIdx);
      handles.erase(removeIt);
      // 2. decrement remaining objects
      auto decrIt = std::next(handles.begin(), handleIdx);
      while (decrIt != handles.end()) {
        auto& obj = *decrIt;
        --obj.handleIdx;
        ++decrIt;
      }
    }
    // TODO: notify listeners here
    return true;
  }
  return false;
}

// Mouse Handling stuff=======================================

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
static lfo_handle_t projectPointToHandle(const frect_t& bounds,
                                         const fpoint_t& point) {
  const float xNorm = (point.getX() - bounds.getX()) / bounds.getWidth();
  const float lvl = (bounds.getBottom() - point.y) / bounds.getHeight();
  auto tableIdx = (size_t)(xNorm * (float)(LFO_SIZE - 1));
  return {tableIdx, lvl};
}

