#pragma once

#include "EditValueTree.h"
#include "Electrum/Identifiers.h"

class WaveEditListener;

class WaveEditorContext : public Component, public ValueTree::Listener {
protected:
  std::vector<WaveEditListener*> watchers;
  void p_setFocusedIndex(int idx);

public:
  ValueTree waveTree;
  WaveEditorContext() { waveTree.addListener(this); }
  ~WaveEditorContext() override { waveTree.removeListener(this); }
  void addListener(WaveEditListener* w);
  void removeListener(WaveEditListener* w);
};

//==================================================================

class WaveEditListener : public Component {
protected:
  ValueTree& waveTree;
  bool isAttached = false;

  String p_getWaveStringForFrame(int frame);

public:
  WaveEditListener(ValueTree& wt) : waveTree(wt) {}
  ~WaveEditListener() override;
  void parentHierarchyChanged() override;
  virtual void frameWasFocused(int) {}
};
