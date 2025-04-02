#include "Electrum/GUI/Util/DragPoint.h"

void DragPoint::removeListener(DragPoint::Listener* l) {
  for (auto it = listeners.begin(); it != listeners.end(); it++) {
    if (*it == l) {
      listeners.erase(it);
      break;
    }
  }
}

void DragPoint::startMove(bool notify) {
  if (notify) {
    for (auto l : listeners)
      l->moveStarted(this);
  }
}
void DragPoint::endMove(bool notify) {
  if (notify) {
    for (auto l : listeners)
      l->moveEnded(this);
  }
}
void DragPoint::movePoint(float x, float y, bool notify) {
  point.x = x;
  point.y = y;
  if (notify) {
    for (auto l : listeners)
      l->moved(this);
  }
}
void DragPoint::moveTo(fpoint_t pt, bool notify) {
  movePoint(pt.x, pt.y, notify);
}
bool DragPoint::isWithin(const juce::MouseEvent& e, float radius) {
  auto ePos = e.position;
  return point.getDistanceFrom(ePos) <= radius;
}

//===================================================
