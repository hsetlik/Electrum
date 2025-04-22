#pragma once
#include "Electrum/Audio/Wavetable.h"
#include "Electrum/Shared/ElectrumState.h"

// a class for custom modal component stuff- editor should override it to
// view/unview modal stuff
class ModalParent : public Component {
public:
  ModalParent() {}
  virtual ~ModalParent() {}
  // these should be overridden
protected:
  virtual void _openWaveEditor(ElectrumState* s, Wavetable* wt, int idx) = 0;
  virtual void _openLFOEditor(ElectrumState* s, int idx) = 0;
  virtual void _exitModalView() = 0;

public:
  // these static methods take a pointer to any added component
  // and find the relevant ModalParent, easier than passing a pointer
  // thru 5 zillion layers of constructors
  static void openWaveEditor(Component* comp,
                             ElectrumState* s,
                             Wavetable* wt,
                             int idx);
  static void openLFOEditor(Component* comp, ElectrumState* s, int idx);
  static void exitModalView(Component* comp);
};
