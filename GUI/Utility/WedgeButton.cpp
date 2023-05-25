#include "WedgeButton.h"

WedgeButton::WedgeButton(const String& name, float startAngle, float endAngle, float innerRadius, float outerRadius) :
Button(name),
aStart(startAngle),
aEnd(endAngle),
r1(innerRadius),
r2(outerRadius)
{

}


void WedgeButton::resized()
{
    auto bounds = getCurrentWedgePath().getBounds();
    setBounds(bounds.toNearestInt());
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
    auto lBounds = getParentComponent()->getLocalBounds().toFloat();
    return GUIUtil::ringSegment(lBounds.getCentreX(), lBounds.getCentreY(), r1, r2, aStart, aEnd);
}