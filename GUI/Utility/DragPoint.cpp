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

void DragPoint::setPoint(float x, float y, bool updateListeners)
{
    fX = xRange.clipValue(x);
    fY = yRange.clipValue(y);
    if (updateListeners)
    {
        for(auto l : listeners)
        {
            l->pointMoved(this);
        }
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

Point<float> DragPoint::getFPointWithin(Component* parent, float cushion)
{
    auto lBounds = parent->getLocalBounds().toFloat().reduced(cushion);
    float x = lBounds.getX() + (fX * lBounds.getWidth());
    float y = lBounds.getY() + (fY * lBounds.getHeight());
    return Point<float>(x, y);
}

bool DragPoint::eventIsWithin(const MouseEvent& e, float radius)
{
    return (e.position.getDistanceFrom(getFPointWithin(e.eventComponent)) < radius);

}

void DragPoint::moveTo(const MouseEvent& e)
{
    auto fBounds = e.eventComponent->getLocalBounds().toFloat();
    float newX = e.position.x / fBounds.getWidth();
    float newY = e.position.y / fBounds.getHeight();
    setPoint(newX, newY);
}
//==========================================================================================

DragPointParameterAttachment::DragPointParameterAttachment(
EVT* tree, 
DragPoint* pt, const String& id, 
std::function<void(float)> onParamChange, 
std::function<float(float,float)> positionToValue) :
state(tree),
point(pt),
paramID(id),
changeCallback(onParamChange),
posToValue(positionToValue)
{
    point->addListener(this);
    auto& param = *state->getAPVTS()->getParameter(paramID);
    paramRange = state->getAPVTS()->getParameterRange(paramID);
    attach.reset(new ParameterAttachment(param, [this] (float f) { changeCallback(f); }));

}

void DragPointParameterAttachment::pointMoved(DragPoint* pt) 
{
    if (pt == point)
    {
        float value = posToValue(pt->getX(), pt->getY());
        attach->setValueAsPartOfGesture(value); 
    }
}

void DragPointParameterAttachment::moveStarted(DragPoint* pt) 
{
    if (pt == point)
    {
        attach->beginGesture();
    }
}
void DragPointParameterAttachment::moveEnded(DragPoint* pt) 
{
    if(pt == point)
    {
        attach->endGesture();
    }
}