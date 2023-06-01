#include "DragPoint.h"
DragPoint::DragPoint(float x, float y, Range<float> xR, Range<float> yR) : 
fX(x), 
fY(y),
xRange(xR),
yRange(yR)
{

}


DragPoint::DragPoint(const DragPoint& other) :
fX(other.fX),
fY(other.fY),
xRange(other.xRange),
yRange(other.yRange)
{

}

void DragPoint::setPoint(float x, float y)
{
    fX = xRange.clipValue(x);
    fY = yRange.clipValue(y);
    for(auto l : listeners)
    {
        l->pointMoved(this, fX, fY);
    }
}

void DragPoint::setXRange(float min, float max)
{
        xRange.setStart(min);
        xRange.setEnd(max);
        if (fX < min)
        {
            setPoint(min, fY);
        }
        else if (fX > max)
        {
            setPoint(max, fY);
        }
}

void DragPoint::setYRange(float min, float max)
{
        yRange.setStart(min);
        yRange.setEnd(max);
        if (fY < min)
        {
            setPoint(fX, min);
        }
        else if (fY > max)
        {
            setPoint(fX, max);
        }
}


Point<int> DragPoint::getIPointWithin(Component* parent)
{
    auto lBounds = parent->getLocalBounds();
    int x = (int) (fX * (float)lBounds.getWidth());
    int y = (int) (fY * (float)lBounds.getHeight());
    return Point<int>(x, y);
}

Point<float> DragPoint::getFPointWithin(Component* parent)
{
    auto lBounds = parent->getLocalBounds().toFloat();
    float x = fX * lBounds.getWidth();
    float y = fY * lBounds.getHeight();
    return Point<float>(x, y);
}

bool DragPoint::isWithin(const MouseEvent& e, float radius)
{
    Rectangle<float> bounds(radius * 2.0f, radius * 2.0f);
    bounds = bounds.withCentre(getFPointWithin(e.eventComponent));
    return bounds.contains(e.position);
}

void DragPoint::moveTo(const MouseEvent& e)
{
    auto fBounds = e.eventComponent->getLocalBounds().toFloat();
    float newX = fBounds.getWidth() / e.position.getX();
    float newY = fBounds.getHeight() / e.position.getY();
    setPoint(newX, newY);
}