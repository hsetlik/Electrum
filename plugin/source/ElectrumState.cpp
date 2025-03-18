
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

//===================================================

ElectrumState::ElectrumState(juce::AudioProcessor& proc,
                             juce::UndoManager* undo)
    : apvts(proc, undo, ID::ELECTRUM_STATE, ID::getParameterLayout()) {
  ValueTree mod(ID::ELECTRUM_MOD_TREE);
  state.appendChild(mod, undo);
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
