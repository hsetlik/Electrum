#include "WedgeButton.h"

WedgeButton::WedgeButton(const String& name, float startAngle, float endAngle, float innerRadius, float outerRadius) :
Button(name),
aStart(startAngle),
aEnd(endAngle),
r1(innerRadius),
r2(outerRadius)
{

}

void WedgeButton::setWedgeParams(float startRads, float endRads, float rad1, float rad2)
{
    aStart = startRads;
    aEnd = endRads;
    r1 = rad1;
    r2 = rad2;
}


void WedgeButton::resizedWithCenter(float x, float y)
{
    auto bounds = getCurrentWedgePath(x, y).getBounds();
    setBounds(bounds.toNearestInt());
}

Path WedgeButton::getCurrentWedgePath(float centerX, float centerY)
{
    return GUIUtil::ringSegment(centerX, centerY, r1, r2, aStart, aEnd);
}