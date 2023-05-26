#include "../../Parameters/ElectrumValueTree.h"
#include "../Color.h"
#define WEDGE_SEGMENTS 256

class WedgeButton : public Button
{
protected:
    static Rectangle<float> getWedgeBounds(float x0, float y0, float startRads, float endRads, float r1, float r2);
    static Path getWedgePath(float x0, float y0, float startRads, float endRads, float r1, float r2);
private:
    float a1, a2, r1, r2
public:
    WedgeButton(const String& name, float startRads=0.0f, float endRads=1.0f, float rad1=1.0f, float rad2=0.0f);
    void setStartAngle(float rads) { a1 = rads; }
    void setEndAngle(float rads) { a2 = rads; }
    void setInnerRadius(float r) { r1 = r; }
    void setOuterRadius(float r) { r2 = r; }

    //call this is the parent's resize method
    void centerOn(Component* parent);
};