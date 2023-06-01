#pragma once
#include "../Color.h"
#include "../../Parameters/MathUtil.h"


class DragPoint
{
protected:
    float fX; // X position relative to the parent's width
    float fY; // Y position relative to the parent's width
    Range<float> xRange; // allowed range for fX
    Range<float> yRange; // allowed range for fY
public:
    float getX() { return fX; }
    float getY() { return fY; }
    void setXRange(float min, float max);
    void setYRange(float min, float max);
    Range<float> getXRange() { return xRange; }
    Range<float> getYRange() { return yRange; }

    // this is the function that calls the listeners
    void setPoint(float x, float y);
    
    DragPoint(float x=0.0f, float y=0.0f, Range<float> xR=Range<float>(0.0f, 0.1f), Range<float>yR=Range<float>(0.0f, 1.0f));
    DragPoint(const DragPoint& other);
    struct Listener
    {
        Listener() {}
        virtual ~Listener() {}
        virtual void pointMoved(DragPoint* point, float newX, float newY)=0; 
    };
    void addListener(Listener* l)
    {
        listeners.push_back(l);
    }
    void removeListener(Listener* l)
    {
        for(auto it = listeners.begin(); it != listeners.end(); it++)
        {
            if (*it == l)
            {
                listeners.erase(it);
                return;
            }
        }
    }
    void operator=(const DragPoint& other)
    {
        fX = other.fX;
        fY = other.fY;
        xRange = other.xRange;
        yRange = other.yRange;
    }
    //========================================================
    Point<int> getIPointWithin(Component* parent);
    Point<float> getFPointWithin(Component* parent);
    bool isWithin(const MouseEvent& e, float radius);
    void moveTo(const MouseEvent& e);
private:
    std::vector<DragPoint::Listener*> listeners;
};
//========================================================================================
