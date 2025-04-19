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
  // children can call this to find out which oscillator is selected
  virtual int getSelectedOscID() = 0;
  // the warp editor should call this on the parent to reload the selected osc
  virtual void previewEditsOnOscillator() = 0;
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
  virtual void waveTreeUpdateRequested() {}
};
