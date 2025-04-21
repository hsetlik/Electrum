#include "Electrum/Audio/Modulator/LFO.h"
#include "Electrum/GUI/GUITypedefs.h"

namespace LFO {

static const String lfoStartToken = "_LFOSTART_";
static const String lfoEndToken = "_LFOEND_";
static const String handleDelim = "_HNDL";
static const String indexDelim = "_IDX";

static String s_handleToString(const lfo_handle_t& h) {
  String out = String(h.tableIdx) + indexDelim;
  out += B64::toString(h.level) + handleDelim;
  return out;
}

static std::vector<String> s_extractHandleStrings(const String& lfoStr) {
  std::vector<String> s = {};
  String handles = lfoStr.trimCharactersAtStart(lfoStartToken);
  handles = handles.trimCharactersAtEnd(lfoEndToken);
  int delimIdx = handles.indexOf(handleDelim);
  while (delimIdx != -1) {
    String hStr = handles.substring(0, delimIdx);
    s.push_back(hStr);
    handles = handles.substring(hStr.length());
    delimIdx = handles.indexOf(handleDelim);
  }
  return s;
}

static lfo_handle_t s_parseHandleString(const String& str) {
  int idxEnd = str.indexOf(indexDelim);
  auto indexString = str.substring(0, idxEnd);
  const size_t tableIdx = (size_t)std::stoi(indexString.toStdString());
  auto remaining = str.substring(idxEnd + indexDelim.length());
  const float lvl = B64::toFloat(remaining);
  return {tableIdx, lvl};
}
//---------------------------------------------------------

void stringDecode(const String& str, std::vector<lfo_handle_t>& dest);
String stringEncode(std::vector<lfo_handle_t>& handles) {
  String out = lfoStartToken;
  for (auto& h : handles) {
    out += s_handleToString(h);
  }
  out += lfoEndToken;
  return out;
}

void stringDecode(const String& str, std::vector<lfo_handle_t>& dest) {
  // 1. clear out the list
  dest.clear();
  // 2. split into handle strings
  auto hStrings = s_extractHandleStrings(str);
  for (auto& handleStr : hStrings) {
    dest.push_back(s_parseHandleString(handleStr));
  }
}

void parseHandlesToTable(const std::vector<lfo_handle_t>& handles,
                         lfo_table_t& dest) {
  jassert(handles.size() > 2);
  size_t leftHandleIdx = 0;
  for (size_t i = 0; i < LFO_SIZE; ++i) {
    jassert(leftHandleIdx < handles.size() - 1);
    auto* lHandle = &handles[leftHandleIdx];
    auto* rHandle = &handles[leftHandleIdx + 1];
    while (rHandle->tableIdx < i) {
      ++leftHandleIdx;
      lHandle = &handles[leftHandleIdx];
      rHandle = &handles[leftHandleIdx + 1];
    }
    const float t = (float)(i - lHandle->tableIdx) /
                    (float)(rHandle->tableIdx - lHandle->tableIdx);
    dest[i] = flerp(lHandle->level, rHandle->level, t);
  }
}
}  // namespace LFO
//===================================================
