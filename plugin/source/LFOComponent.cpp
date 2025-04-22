#include "Electrum/GUI/ModulatorPanel/LFOComponent.h"
#include "Electrum/Audio/Modulator/LFO.h"
#include "Electrum/Identifiers.h"
LFOThumbnail::LFOThumbnail(ElectrumState* s, int idx) : state(s), lfoID(idx) {
  shapeStringID = ID::lfoShapeString.toString() + String(lfoID);
  loadShapePoints();
}

void LFOThumbnail::loadShapePoints() {
  auto lfoTree = state->state.getChildWithName(ID::LFO_INFO);
  jassert(lfoTree.isValid());
  String _shapeStr = lfoTree[shapeStringID];
  std::vector<lfo_handle_t> handles = {};
  LFO::stringDecode(_shapeStr, handles);
  shapePts.clear();
  for (auto& h : handles) {
    float x = (float)h.tableIdx / (float)(LFO_SIZE - 1);
    shapePts.push_back({x, h.level});
  }
}

void LFOThumbnail::graphingDataUpdated(GraphingData* gd) {
  const float _phase = gd->getLFOPhase(lfoID);
  if (!fequal(_phase, currentPhase)) {
    currentPhase = _phase;
    repaint();
  }
}
//===================================================
