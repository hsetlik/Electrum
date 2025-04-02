#pragma once

// NOTE: this class should be strictly for display logic, should not directly
// read or write shared state!
#include "Electrum/Identifiers.h"
#include "../GUITypedefs.h"
struct DragPoint {
public:
  Component* const parent;

private:
  fpoint_t point;

public:
  struct Listener {
    virtual ~Listener() {}
    virtual void moveStarted(DragPoint* p) = 0;
    virtual void moved(DragPoint* p) = 0;
    virtual void moveEnded(DragPoint* p) = 0;
  };

  fpoint_t getPos() const { return point; }

  void addListener(Listener* l) { listeners.push_back(l); }

  void removeListener(Listener* l);
  DragPoint(Component* p, float x = 0.0f, float y = 0.0f)
      : parent(p), point(x, y) {}
  float xF() { return point.getX() / (float)parent->getWidth(); }
  float yF() { return point.getY() / (float)parent->getHeight(); }
  float getX() { return point.x; }
  float getY() { return point.y; }
  // these are the functions that (optionally) call the listeners
  // NOTE: the 'notify' parameter should be false when calling these methods
  // from the atttachment class
  void startMove(bool notify = true);
  void endMove(bool notify = true);
  void movePoint(float x, float y, bool notify = true);
  void moveTo(fpoint_t pt, bool notify = true);
  bool isWithin(const juce::MouseEvent& e, float radius);

private:
  std::vector<Listener*> listeners;
};

