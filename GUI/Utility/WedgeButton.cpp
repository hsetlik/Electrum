#include "WedgeButton.h"

WedgeButton::WedgeButton(const String& name, float startAngle, float endAngle, float innerRadius, float outerRadius) :
Button(name),
aStart(startAngle),
aEnd(endAngle),
r1(innerRadius),
r2(outerRadius)
{

}


void WedgeButton::resizeWedge(float startAngle, float endAngle, float innerRadius, float outerRadius)
{
    auto lBounds = getParentComponent()->getLocalBounds().toFloat();
    auto wedgeBounds = getWedgeBounds(lBounds.getCentreX(), lBounds.getCentreY(), startAngle, endAngle, innerRadius, outerRadius);
    DLog::log("Wedge bounds: " + wedgeBounds.toString());
    setBounds(wedgeBounds.toNearestInt());
}

void WedgeButton::resized()
{
    resizeWedge(aStart, aEnd, r1, r2);
}

Rectangle<float> WedgeButton::getWedgeBounds(float x0, float y0, float startRads, float endRads, float r1, float r2)
{
    jassert(r2 >= r1);
    const float x1Inner = std::sin(startRads) * r1;
    const float x1Outer = std::sin(startRads) * r2;
    const float y1Inner = std::cos(startRads) * r1;
    const float y1Outer = std::cos(startRads) * r2;

    const float x2Inner = std::sin(endRads) * r1;
    const float x2Outer = std::sin(endRads) * r2;
    const float y2Inner = std::cos(endRads) * r1;
    const float y2Outer = std::cos(endRads) * r2;

    const float x = x0 + std::min({x1Inner, x1Outer, x2Inner, x2Outer});
    const float width = (x0 + std::max({x1Inner, x1Outer, x2Inner, x2Outer})) - x;
    const float y = y0 + std::min({y1Inner, y1Outer, y2Inner, y2Outer});
    const float height = (y0 + std::max({y1Inner, y1Outer, y2Inner, y2Outer})) - y;

    return {x, y, width, height};
}

void WedgeButton::setWedgeParams(float startRads, float endRads, float rad1, float rad2)
{
    aStart = startRads;
    aEnd = endRads;
    r1 = rad1;
    r2 = rad2;
}


Path WedgeButton::getCurrentWedgePath()
{
    auto lBounds = getParentComponent()->getBounds().toFloat();
    return GUIUtil::ringSegment(lBounds.getCentreX(), lBounds.getCentreY(), r1, r2, aStart, aEnd);
}