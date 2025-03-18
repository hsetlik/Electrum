
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

void ModMap::updateMap(ValueTree& modTree) {
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

//===================================================

ElectrumState::ElectrumState(juce::AudioProcessor& proc,
                             juce::UndoManager* undo)
    : apvts(proc, undo, ID::ELECTRUM_STATE, ID::getParameterLayout()) {}
