#include "Electrum/GUI/Modulation/ModContextComponent.h"
#include "Electrum/Identifiers.h"
#include "Electrum/Shared/ElectrumState.h"

ModDestAttachment::ModDestAttachment(int id) : destID(id) {}

void ModDestAttachment::parentHierarchyChanged() {
  auto* context = findParentComponentOfClass<ModContextComponent>();
  if (context != nullptr) {
    context->addDestListener(this);
    isAttached = true;
    // also needs to check for reinit here
    if (context->needsReinit[(size_t)destID]) {
      reinit();
      context->needsReinit[(size_t)destID] = false;
    }
  }
}

//===================================================

ModContextComponent::ModContextComponent(ElectrumState* mainTree)
    : state(mainTree), needsReinit({}) {
  state->state.addListener(this);
  // check for any modulations that were added BEFORE
  // this component (and thus this listener) were attached
  auto modTree = state->getModulationTree();
  for (auto it = modTree.begin(); it != modTree.end(); ++it) {
    auto child = *it;
    jassert(child.hasType(ID::ELECTRUM_MODULATION));
    int id = child[ID::modDestID];
    needsReinit[(size_t)id] = true;
  }
}

ModContextComponent::~ModContextComponent() {
  state->state.removeListener(this);
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
//
// static std::vector<mod_src_t> modsForDest(const ValueTree& modTree,
//                                           int destID) {
//   std::vector<mod_src_t> vec = {};
//   for (auto it = modTree.begin(); it != modTree.end(); ++it) {
//     auto mod = *it;
//     const int dest = mod[ID::modDestID];
//     if (dest == destID) {
//       const int src = mod[ID::modSourceID];
//       const float depth = mod[ID::modDepth];
//       vec.push_back({src, depth});
//     }
//   }
//   return vec;
// }
//
void ModContextComponent::valueTreeRedirected(ValueTree& changed) {
  auto modTree = changed.getChildWithName(ID::ELECTRUM_MOD_TREE);
  jassert(modTree.isValid());
  for (auto* l : dListeners) {
    l->reinit();
  }
}
