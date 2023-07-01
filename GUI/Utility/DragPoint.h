#pragma once
#include "../Color.h"
#include "../../Parameters/MathUtil.h"
#include "../../Parameters/ElectrumValueTree.h"

//NOTE: this class should be strictly for display logic, should not directly read or write shared state!
struct DragPoint
{
public:
    Component* const parent;
private:
    Point<float> point;
public:
    struct Listener
    {
      virtual ~Listener(){}
      virtual void moveStarted(DragPoint* p)=0;
      virtual void moved(DragPoint* p)=0;
      virtual void moveEnded(DragPoint* p)=0;
    };

    Point<float> getPos() const
    {
      return point;
    }

    void addListener(Listener* l)
    {
      listeners.push_back(l);
    }

    void removeListener(Listener* l)
    {
      for(auto it = listeners.begin(); it != listeners.end(); it++)
      {
        if(*it == l)
        {
          listeners.erase(it);
          break;
        }
      }
    }
    DragPoint(Component* p, float x=0.0f, float y=0.0f) :
    parent(p),
    point(x, y)
    {
    }
    float xF()
    {
        return point.getX() / (float)parent->getWidth();
    }
    float yF()
    {
        return point.getY() / (float)parent->getHeight();
    }
    float getX() { return point.x; }
    float getY() { return point.y; }
    // these are the functions that (optionally) call the listeners
    // NOTE: the 'notify' parameter should be false when calling these methods from the atttachment class
    void startMove(bool notify=true)
    {
      if(notify)
      {
        for(auto l : listeners)
          l->moveStarted(this);
      }
    }
    void endMove(bool notify=true)
    {
      if(notify)
      {
        for(auto l : listeners)
          l->moveEnded(this);
      }
    }
    void movePoint(float x, float y, bool notify = true)
    {
      point.x = x;
      point.y = y;
      if(notify)
      {
        for(auto l : listeners)
          l->moved(this);
      }
    }
    void moveTo(Point<float> pt, bool notify=true)
    {
      movePoint(pt.x, pt.y, notify);
    }
    bool isWithin(const MouseEvent& e, float radius)
    {
      auto ePos = e.position;
      return point.getDistanceFrom(ePos) <= radius;
    }
private: 
    std::vector<Listener*> listeners;
};

//====================================================================================
// these are the two functions that each attachment will need to define
using PosToParamFunc = std::function<float(Point<float>)>;
using ParamToPosFunc = std::function<Point<float>(float)>;

// This does the parameter handling
class DragPointAttachment : public DragPoint::Listener
{
  private:
    EVT* const state;
    const String paramID;
    DragPoint* const point;
    PosToParamFunc posToParam;
    ParamToPosFunc paramToPos;
    std::unique_ptr<ParameterAttachment> pAttach;
    std::atomic<bool> isMoving;
  public:
    DragPointAttachment(EVT* tree, const String& param, DragPoint* p, PosToParamFunc func1, ParamToPosFunc func2);
    ~DragPointAttachment() override;
    // listener overrides
    void moveStarted(DragPoint* p) override;
    void moveEnded(DragPoint* p) override;
    void moved(DragPoint* p) override;
};
