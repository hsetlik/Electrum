#include "Electrum/GUI/Modulation/ModContextComponent.h"
#include "Electrum/Identifiers.h"

ModDestListener::ModDestListener(int id) : destID(id) {
  auto* context = findParentComponentOfClass<ModContextComponent>();
  jassert(context != nullptr);
  context->addDestListener(this);
}
//===================================================

ModContextComponent::ModContextComponent(ElectrumState* mainTree)
    : state(mainTree) {
  auto modTree = state->getModulationTree();
  modTree.addListener(this);
}

ModContextComponent::~ModContextComponent() {
  auto modTree = state->getModulationTree();
  modTree.removeListener(this);
}

void ModContextComponent::valueTreePropertyChanged(
    ValueTree& tree,
    const juce::Identifier& propID) {
  jassert(tree.hasType(ID::ELECTRUM_MODULATION));
  if (propID == ID::modDepth) {
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
  jassert(parent.hasType(ID::ELECTRUM_MOD_TREE));
  const int dest = child[ID::modDestID];
  const int src = child[ID::modSourceID];
  const float depth = child[ID::modDepth];
  for (auto* d : dListeners) {
    if (d->destID == dest) {
      d->sourceAdded(src, depth);
    }
  }
}

void ModContextComponent::valueTreeChildRemoved(ValueTree& parent,
                                                ValueTree& child,
                                                int index) {
  juce::ignoreUnused(index);
  jassert(parent.hasType(ID::ELECTRUM_MOD_TREE));
  const int dest = child[ID::modDestID];
  const int src = child[ID::modSourceID];
  for (auto* d : dListeners) {
    if (d->destID == dest) {
      d->sourceRemoved(src);
    }
  }
}

