#pragma once

#include "../../Common.h"
#include "Electrum/Shared/ElectrumState.h"
#include "juce_core/juce_core.h"

class ModDestListener;
//

/* The top-level component should inherit from this
 * such that all the other modulation-related components
 * have something to talk to
 * */
class ModContextComponent : public Component, public ValueTree::Listener {
private:
  ElectrumState* const state;
  std::vector<ModDestListener*> dListeners;

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
  // add dest listeners like so
  void addDestListener(ModDestListener* l) { dListeners.push_back(l); }
};

// components that can be modulated should inherit from this
class ModDestListener : public Component {
public:
  const int destID;
  ModDestListener(int id);
  virtual ~ModDestListener() {}
  virtual void sourceAdded(int src, float depth) {
    juce::ignoreUnused(src, depth);
  }
  virtual void sourceChanged(int src, float depth) {
    juce::ignoreUnused(src, depth);
  }
  virtual void sourceRemoved(int src) { juce::ignoreUnused(src); }
  virtual void reinit(std::vector<mod_src_t> sources) {
    juce::ignoreUnused(sources);
  }
};

