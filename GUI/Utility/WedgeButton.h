#pragma once
#include "../../Parameters/ElectrumValueTree.h"
#include "../Color.h"

// A slice of two co-centric circles centered around the WedgeButton's parent component
class WedgeButton : public Button
{
public:
    WedgeButton(const String& name, float startAngle=0.0f, float endAngle=0.0f, float innerRadius=1.0f, float outerRadius=2.0f);
    void resized() override;
    void setWedgeParams(float startRads, float endRads, float r1, float r2);
private:
    float aStart, aEnd, r1, r2;

protected:
    Path getCurrentWedgePath();

};