#pragma once
#include "../Color.h"
#include "../../Parameters/MathUtil.h"
#include "../../Parameters/ElectrumValueTree.h"

//NOTE: this class should be strictly for display logic, should not directly read or write shared state!
class DragPoint
{
    Component* const parent;
    Point<float> point;
public:
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
};
