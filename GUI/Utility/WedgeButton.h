#pragma once
#include "../../Parameters/ElectrumValueTree.h"
#include "../Color.h"

// A slice of two co-centric circles centered around the WedgeButton's parent component
class WedgeButton : public Button
{
public:
    WedgeButton(const String& name, float startAngle=0.0f, float endAngle=0.0f, float innerRadius=1.0f, float outerRadius=2.0f);
    void resizeWedge(float startAngle, float endAngle, float innerRadius, float outerRadius);
    void resized() override;
    void setWedgeParams(float startRads, float endRads, float r1, float r2);
private:
    float aStart, aEnd, r1, r2;
// math for calculating the bounds of a wedge centered around a given point
    static Rectangle<float> getWedgeBounds(float x0, float y0, float startRads, float endRads, float rad1, float rad2);
protected:
    Path getCurrentWedgePath();

};