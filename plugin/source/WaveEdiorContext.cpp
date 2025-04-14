#include "Electrum/GUI/WaveEditor/WaveEdiorContext.h"

void WaveEditorContext::addListener(WaveEditListener* w) {
  watchers.push_back(w);
}

void WaveEditorContext::removeListener(WaveEditListener* w) {
  for (auto it = watchers.begin(); it != watchers.end(); ++it) {
    if (*it == w) {
      watchers.erase(it);
      return;
    }
  }
}

void WaveEditorContext::p_setFocusedIndex(int idx) {
  for (auto* l : watchers) {
    l->frameWasFocused(idx);
  }
}
//===================================================

WaveEditListener::~WaveEditListener() {
  if (isAttached) {
    auto* context = findParentComponentOfClass<WaveEditorContext>();
    if (context != nullptr)
      context->removeListener(this);
  }
}

void WaveEditListener::parentHierarchyChanged() {
  if (!isAttached) {
    auto* context = findParentComponentOfClass<WaveEditorContext>();
    if (context != nullptr) {
      context->addListener(this);
      isAttached = true;
    }
  }
}
