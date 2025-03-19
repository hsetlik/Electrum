#include "Electrum/GUI/Modulation/ModContextComponent.h"
#include "Electrum/Identifiers.h"
#include "Electrum/Shared/ElectrumState.h"

ModDestListener::ModDestListener(int id) : destID(id) {
  auto* context = findParentComponentOfClass<ModContextComponent>();
  jassert(context != nullptr);
  context->addDestListener(this);
}
//===================================================

ModContextComponent::ModContextComponent(ElectrumState* mainTree)
    : state(mainTree) {
  state->state.addListener(this);
}

ModContextComponent::~ModContextComponent() {
  auto modTree = state->getModulationTree();
  modTree.removeListener(this);
}

void ModContextComponent::valueTreePropertyChanged(
    ValueTree& tree,
    const juce::Identifier& propID) {
  if (tree.hasType(ID::ELECTRUM_MODULATION) && propID == ID::modDepth) {
    const int src = tree[ID::modSourceID];
    const int dest = tree[ID::modDestID];
    const float depth = tree[ID::modDepth];
    for (auto* d : dListeners) {
      if (d->destID == dest) {
        d->sourceChanged(src, depth);
      }
    }
  }
}

void ModContextComponent::valueTreeChildAdded(ValueTree& parent,
                                              ValueTree& child) {
  if (parent.hasType(ID::ELECTRUM_MOD_TREE)) {
    const int dest = child[ID::modDestID];
    const int src = child[ID::modSourceID];
    const float depth = child[ID::modDepth];
    for (auto* d : dListeners) {
      if (d->destID == dest) {
        d->sourceAdded(src, depth);
      }
    }
  }
}

void ModContextComponent::valueTreeChildRemoved(ValueTree& parent,
                                                ValueTree& child,
                                                int index) {
  juce::ignoreUnused(index);
  if (parent.hasType(ID::ELECTRUM_MOD_TREE)) {
    const int dest = child[ID::modDestID];
    const int src = child[ID::modSourceID];
    for (auto* d : dListeners) {
      if (d->destID == dest) {
        d->sourceRemoved(src);
      }
    }
  }
}

static std::vector<mod_src_t> modsForDest(const ValueTree& modTree,
                                          int destID) {
  std::vector<mod_src_t> vec = {};
  for (auto it = modTree.begin(); it != modTree.end(); ++it) {
    auto mod = *it;
    const int dest = mod[ID::modDestID];
    if (dest == destID) {
      const int src = mod[ID::modSourceID];
      const float depth = mod[ID::modDepth];
      vec.push_back({src, depth});
    }
  }
  return vec;
}

void ModContextComponent::valueTreeRedirected(ValueTree& changed) {
  auto modTree = changed.getChildWithName(ID::ELECTRUM_MOD_TREE);
  jassert(modTree.isValid());
  for (auto* l : dListeners) {
    auto sources = modsForDest(modTree, l->destID);
    l->reinit(sources);
  }
}

