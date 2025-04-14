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
DragPointParamAttach::DragPointParamAttach(ElectrumState* s,
                                           const String& id,
                                           DragPoint* p,
                                           PosToParamFunc f1,
                                           ParamToPosFunc f2)
    : state(s), point(p), paramID(id), posToParam(f1), paramToPos(f2) {
  point->addListener(this);
  auto callback = [this](float value) {
    auto pos = paramToPos(value);
    point->movePoint(pos.x, pos.y, false);
  };
  auto param = state->getParameter(paramID);
  pAttach.reset(
      new juce::ParameterAttachment(*param, callback, state->undoManager));
  pAttach->sendInitialUpdate();
}

DragPointParamAttach::~DragPointParamAttach() {
  point->removeListener(this);
}

void DragPointParamAttach::moveStarted(DragPoint*) {
  pAttach->beginGesture();
  isMoving = true;
}

void DragPointParamAttach::moveEnded(DragPoint*) {
  if (isMoving.load()) {
    pAttach->endGesture();
    isMoving = false;
  }
}

void DragPointParamAttach::moved(DragPoint* p) {
  auto val = posToParam({p->getX(), p->getY()});
  pAttach->setValueAsPartOfGesture(val);
}
