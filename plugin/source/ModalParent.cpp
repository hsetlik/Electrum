#include "Electrum/GUI/Util/ModalParent.h"

void ModalParent::openWaveEditor(Component* comp,
                                 ElectrumState* s,
                                 Wavetable* wt,
                                 int idx) {
  auto* ptr = comp->findParentComponentOfClass<ModalParent>();
  jassert(ptr != nullptr);
  ptr->_openWaveEditor(s, wt, idx);
}

void ModalParent::exitModalView(Component* comp) {
  auto* ptr = comp->findParentComponentOfClass<ModalParent>();
  ptr->_exitModalView();
}
//===================================================
