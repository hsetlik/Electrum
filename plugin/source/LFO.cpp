#include "Electrum/Audio/Modulator/LFO.h"
#include "Electrum/GUI/GUITypedefs.h"
#include "Electrum/Identifiers.h"
#include "Electrum/Shared/ElectrumState.h"

namespace LFO {

static const String lfoStartToken = "_LFOSTART_";
static const String lfoEndToken = "_LFOEND_";
static const String handleDelim = "_HNDL";
static const String indexDelim = "_IDX";

static String s_handleToString(const lfo_handle_t& h) {
  String out = B64::toString((size_t)h.tableIdx) + indexDelim;
  out += B64::toString(h.level) + handleDelim;
  return out;
}

static std::vector<String> s_extractHandleStrings(const String& lfoStr) {
  std::vector<String> s = {};
  String handles = lfoStr.substring(lfoStartToken.length());
  int delimIdx = handles.indexOf(handleDelim);
  while (delimIdx != -1) {
    String hStr = handles.substring(0, delimIdx);
    s.push_back(hStr);
    handles = handles.substring(hStr.length() + handleDelim.length());
    delimIdx = handles.indexOf(handleDelim);
  }
  jassert(s.size() > 1);
  return s;
}

static lfo_handle_t s_parseHandleString(const String& str) {
  int idxEnd = str.indexOf(indexDelim);
  auto indexString = str.substring(0, idxEnd);
  const int tableIdx = (int)B64::toSizeT(indexString);
  auto remaining = str.substring(idxEnd + indexDelim.length());
  const float lvl = B64::toFloat(remaining);
  return {tableIdx, lvl};
}
//---------------------------------------------------------

String stringEncode(handle_vector_t& handles) {
  String out = lfoStartToken;
  for (auto& h : handles) {
    out += s_handleToString(h);
  }
  out += lfoEndToken;
  return out;
}

void stringDecode(const String& str, handle_vector_t& dest) {
  // 1. clear out the list
  dest.clear();
  // 2. split into handle strings
  auto hStrings = s_extractHandleStrings(str);
  for (auto& handleStr : hStrings) {
    dest.push_back(s_parseHandleString(handleStr));
  }
}

void parseHandlesToTable(const handle_vector_t& handles, lfo_table_t& dest) {
  jassert(handles.size() >= 2);
  size_t leftHandleIdx = 0;
  bool wrapped = false;
  for (size_t i = 0; i < LFO_SIZE; ++i) {
    jassert(leftHandleIdx < handles.size());
    auto* lHandle = &handles[leftHandleIdx];
    auto* rHandle = &handles[leftHandleIdx + 1];
    while (rHandle->tableIdx <= (int)i && !wrapped) {
      ++leftHandleIdx;
      if (leftHandleIdx < handles.size() - 1) {
        lHandle = &handles[leftHandleIdx];
        rHandle = &handles[leftHandleIdx + 1];
      } else {
        lHandle = &handles[leftHandleIdx];
        rHandle = &handles[0];
        wrapped = true;
      }
    }
    const float t = (float)((int)i - lHandle->tableIdx) /
                    (float)(rHandle->tableIdx - lHandle->tableIdx);
    dest[i] = flerp(lHandle->level, rHandle->level, t);
  }
}
}  // namespace LFO
//===================================================

LowFrequencyLUT::LowFrequencyLUT() {
  // zero out the tables
  table1.fill(0.0f);
  table2.fill(0.0f);

  phaseDelt = (float)((double)lfoHz / SampleRate::get());
  startTimerHz(LFO_UPDATE_HZ);
}

void LowFrequencyLUT::timerCallback() {
  needsData = true;
}

void LowFrequencyLUT::updateData(apvts& tree, int idx) {
  if (needsData) {
    auto childTree = tree.state.getChildWithName(ID::LFO_INFO);
    if (childTree.isValid()) {
      String propID = ID::lfoShapeString.toString() + String(idx);
      const String _lfoStr = childTree[propID];
      const size_t _lfoHash = _lfoStr.hash();
      if (_lfoHash != currentLfoHash) {
        currentLfoHash = _lfoHash;
        currentLfoString = _lfoStr;
        triggerAsyncUpdate();
      }
      needsData = false;
    } else {
      jassert(false);
    }
  }
}

void LowFrequencyLUT::handleAsyncUpdate() {
  // 1. decode the string into LFO handles
  LFO::stringDecode(currentLfoString, handles);
  // 2. parse those handles into the idle table
  LFO::parseHandlesToTable(handles, *tIdle);
  // 3. switch the pointers
  auto* prevActive = tActive;
  tActive = tIdle;
  tIdle = prevActive;
}

void LowFrequencyLUT::tick() {
  if (trigMode == LFOTriggerE::Global) {
    globalPhase += phaseDelt;
    if (globalPhase > 1.0f) {
      globalPhase -= 1.0f;
    }
  }
}

float LowFrequencyLUT::getSample(float normPhase) const {
  auto phase = (trigMode == LFOTriggerE::Global) ? globalPhase : normPhase;
  const size_t idx = (size_t)(phase * (float)(LFO_SIZE - 1));
  const lfo_table_t& arr = *tActive;
  return arr[idx];
}

float LowFrequencyLUT::processSample(float& currentPhase) const {
  if (trigMode == LFOTriggerE::Global) {
    return getSample(globalPhase);
  } else {
    currentPhase += phaseDelt;
    if (currentPhase > 1.0f) {
      currentPhase -= 1.0f;
    }
    return getSample(currentPhase);
  }
}

//===============================================================================

VoiceLFO::VoiceLFO(LowFrequencyLUT* l) : lut(l) {}

void VoiceLFO::tick() {
  lastOutput = lut->processSample(phase);
}

void VoiceLFO::gateStarted() {
  switch (lut->getTriggerMode()) {
    case Global:
      return;
    case RetrigStart:
      phase = 0.0f;
      return;
    case RetrigRand:
      phase = rng.nextFloat();
      return;
  }
}

float VoiceLFO::getCurrentPhase() const {
  if (lut->getTriggerMode() == LFOTriggerE::Global) {
    return lut->getGlobalPhase();
  }
  return phase;
}
