
#include "Electrum/Shared/ElectrumState.h"
#include "Electrum/Identifiers.h"
ModMap::ModMap() {
  for (auto& dest : depthArr) {
    dest.fill(0.0f);
  }
  for (auto& dest : boolArr) {
    dest.fill(false);
  }
}

void ModMap::clearBoolGrid() {
  for (auto& dest : boolArr) {
    dest.fill(false);
  }
}

void ModMap::updateMap(ValueTree modTree) {
  jassert(modTree.hasType(ID::ELECTRUM_MOD_TREE));
  clearBoolGrid();
  for (auto it = modTree.begin(); it != modTree.end(); ++it) {
    auto child = *it;
    jassert(child.hasType(ID::ELECTRUM_MODULATION));
    const int src = child[ID::modSourceID];
    const int dest = child[ID::modDestID];
    const float depth = child[ID::modDepth];
    boolArr[(size_t)src][(size_t)dest] = true;
    depthArr[(size_t)src][(size_t)dest] = depth;
  }
}

bool ModMap::modExists(int src, int dest) const {
  return boolArr[(size_t)src][(size_t)dest];
}

int ModMap::numSourcesOnDest(int _dest) const {
  size_t dest = (size_t)_dest;
  int count = 0;
  for (size_t src = 0; src < MOD_SOURCES; ++src) {
    if (boolArr[src][dest]) {
      ++count;
    }
  }
  return count;
}

std::vector<mod_src_t> ModMap::getSourcesFor(int _dest) {
  size_t dest = (size_t)_dest;
  std::vector<mod_src_t> vec = {};
  for (size_t src = 0; src < MOD_SOURCES; ++src) {
    if (boolArr[src][dest]) {
      vec.push_back({(int)src, depthArr[src][dest]});
    }
  }
  return vec;
}

/*This loads the only the modulations in use into the first part of the
 * 'moc_src_t' array at 'arr' and sets the value of 'numSources' to
 * the number of sources in use*/
void ModMap::getSourcesSafe(mod_src_t* arr, int* numSources, int destID) const {
  *numSources = 0;
  for (size_t src = 0; src < MOD_SOURCES; ++src) {
    if (boolArr[src][(size_t)destID]) {
      auto* srcPtr = &arr[*numSources];
      srcPtr->source = (int)src;
      srcPtr->depth = depthArr[src][(size_t)destID];
      *numSources += 1;
    }
  }
}

//===================================================

static std::array<String, MOD_DESTS> _getModDestParamIDs() {
  std::array<String, MOD_DESTS> arr;
  size_t modDest = 0;
  int oscIdx = 0;
  while (modDest < MOD_DESTS) {
    if (modDest <= ModDestE::osc3Pos) {
      String iStr(oscIdx);
      String coarseID = ID::oscillatorCoarseTune.toString() + iStr;
      arr[modDest] = coarseID;
      ++modDest;
      String fineID = ID::oscillatorFineTune.toString() + iStr;
      arr[modDest] = fineID;
      ++modDest;
      String posID = ID::oscillatorPos.toString() + iStr;
      arr[modDest] = posID;
      ++modDest;
      String levelID = ID::oscillatorLevel.toString() + iStr;
      arr[modDest] = levelID;
      ++modDest;
      String panID = ID::oscillatorPan.toString() + iStr;
      arr[modDest] = panID;
      ++modDest;
      ++oscIdx;
    }
  }
  return arr;
}

String _paramIDForModDest(int destID) {
  static std::array<String, MOD_DESTS> destIDs = _getModDestParamIDs();
  return destIDs[(size_t)destID];
}

ElectrumState::ElectrumState(juce::AudioProcessor& proc,
                             juce::UndoManager* undo)
    : apvts(proc, undo, ID::ELECTRUM_STATE, ID::getParameterLayout()) {
  // 1. add the default modulation tree
  ValueTree mod(ID::ELECTRUM_MOD_TREE);
  state.appendChild(mod, undo);
  // now we initialize the modDestRanges array
  // remember this is in order of the ModDestE enum
  for (int i = 0; i < MOD_DESTS; ++i) {
    modDestRanges[i] = getParameterRange(_paramIDForModDest(i));
  }
  // 2. set the wavetable paths for each of the oscs
  for (int i = 0; i < NUM_OSCILLATORS; ++i) {
    juce::Identifier id = ID::oscWavePath.toString() + String(i);
    state.setProperty(id, "Default", undo);
  }
}

float ElectrumState::getModulatedDestValue(int destID,
                                           float baseValue,
                                           float modNorm) const {
  float bNorm = modDestRanges[destID].convertTo0to1(baseValue);
  bNorm = std::clamp(bNorm + modNorm, 0.0f, 1.0f);
  return modDestRanges[destID].convertFrom0to1(bNorm);
}

ValueTree ElectrumState::findTreeForRouting(const ValueTree& modTree,
                                            int src,
                                            int dest) {
  for (auto it = modTree.begin(); it != modTree.end(); ++it) {
    auto mod = *it;
    jassert(mod.hasType(ID::ELECTRUM_MODULATION));
    int _src = mod[ID::modSourceID];
    int _dest = mod[ID::modDestID];
    if (src == _src && dest == _dest)
      return mod;
  }
  return ValueTree();
}

void ElectrumState::setModulation(int src, int dest, float depth) {
  auto modTree = getModulationTree();
  auto child = findTreeForRouting(modTree, src, dest);
  if (child.isValid()) {  // this modulation already exists, we just update its
                          // depth
    child.setProperty(ID::modDepth, depth, undoManager);
  } else {  // create a new child for the modulation
    ValueTree newMod(ID::ELECTRUM_MODULATION);
    newMod.setProperty(ID::modSourceID, src, undoManager);
    newMod.setProperty(ID::modDestID, dest, undoManager);
    newMod.setProperty(ID::modDepth, depth, undoManager);
    modTree.appendChild(newMod, undoManager);
  }
}

void ElectrumState::removeModulation(int src, int dest) {
  auto modTree = getModulationTree();
  auto child = findTreeForRouting(modTree, src, dest);
  jassert(child.isValid());
  modTree.removeChild(child, undoManager);
}

bool ElectrumState::modulationExists(int src, int dest) {
  auto modTree = getModulationTree();
  for (auto it = modTree.begin(); it != modTree.end(); ++it) {
    auto mod = *it;
    const int _src = mod[ID::modSourceID];
    const int _dest = mod[ID::modDestID];
    if (_src == src && _dest == dest)
      return true;
  }
  return false;
}

float ElectrumState::modulationDepth(int src, int dest) {
  auto modTree = getModulationTree();
  auto mod = findTreeForRouting(modTree, src, dest);
  jassert(mod.isValid());
  return (float)mod[ID::modDepth];
}
