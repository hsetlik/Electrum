#pragma once

#include "../../Common.h"
#include "Electrum/Shared/ElectrumState.h"
#include "juce_core/juce_core.h"

class ModDestAttachment;
//

/* The top-level component should inherit from this
 * such that all the other modulation-related components
 * have something to talk to
 * */
class ModContextComponent : public Component, public ValueTree::Listener {
private:
  ElectrumState* const state;
  std::vector<ModDestAttachment*> dListeners;

public:
  ModContextComponent(ElectrumState* mainTree);
  ~ModContextComponent() override;
  // listener overrides
  void valueTreePropertyChanged(ValueTree& tree,
                                const juce::Identifier& propID) override;
  void valueTreeChildAdded(ValueTree& parent, ValueTree& child) override;
  void valueTreeChildRemoved(ValueTree& parent,
                             ValueTree& child,
                             int index) override;
  void valueTreeRedirected(ValueTree& changedTree) override;
  // add dest listeners like so
  void addDestListener(ModDestAttachment* l) { dListeners.push_back(l); }
};
//============================================================

// components that can be modulated should inherit from this
class ModDestAttachment : public Component {
public:
  const int destID;
  ModDestAttachment(int id);
  virtual ~ModDestAttachment() override {}
  virtual void sourceAdded(int src, float depth) {
    juce::ignoreUnused(src, depth);
  }
  virtual void sourceChanged(int src, float depth) {
    juce::ignoreUnused(src, depth);
  }
  virtual void sourceRemoved(int src) { juce::ignoreUnused(src); }
  virtual void reinit() {}
  void parentHierarchyChanged() override;

protected:
  bool isAttached = false;
  void _attemptAttach();
};
