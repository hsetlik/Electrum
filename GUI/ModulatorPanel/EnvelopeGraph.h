#pragma once
#include "../../Parameters/ElectrumValueTree.h"
#include "../Color.h"
#include "../Utility/DragPoint.h"
/*
Current todo for this:
1. fix the issue with out-of-range conversions between FATTACK etc. type scaling and the actual APVTS range skews
2. wire up the three curve points to their parameters
3. draw the appropriate exponential curves on the graph
*/
#define FATTACK 0.2f
#define FHOLD 0.1f
#define FDECAY 0.3f
#define FRELEASE 0.4f

#define HANDLE_STROKE 0.75f
#define HOLD_BUF 4.0f
class EnvelopeGraph : 
public Component,
public AsyncUpdater
{
    // just some static helpers to figure things out. . .
    static float getMaxAttackLength(Rectangle<float>& bounds)
    {
        return FATTACK * bounds.getWidth();
    }

    static float getMaxHoldLength(Rectangle<float>& bounds)
    {
        return FHOLD * bounds.getWidth();
    }
    static float getMaxDecayLength(Rectangle<float>& bounds)
    {
        return FDECAY * bounds.getWidth();
    }
    static float getMaxReleaseLength(Rectangle<float>& bounds)
    {
        return FRELEASE * bounds.getWidth();
    }
    static float sustainLevelY(Rectangle<float>& bounds, float val)
    {
        float min = bounds.getY() + 5.0f;
        float max = bounds.getBottom() - 5.0f;
        return jmap(1.0f - val, 0.0f, 1.0f, min, max);
    }
    static float sustainFromY(Rectangle<float>& bounds, float y)
    {
       float min = bounds.getY() + 5.0f;
       float max = bounds.getBottom() - 5.0f;
       return 1.0f - jmap(y, min, max, 0.0f, 1.0f);
    }
    void drawEnvelopeGraph(Rectangle<float>& bounds, Graphics& g);
    EVT* const state;
public:
    const int index;
    EnvelopeGraph(EVT* tree, int idx);
    void paint(Graphics& g) override;
    void handleAsyncUpdate() override;
    // mouse callbacks
    void mouseDown(const MouseEvent& e) override;
    void mouseDrag(const MouseEvent& e) override;
    void mouseUp(const MouseEvent& e) override;
    // make sure things are synced with the state when the component is brought into focus
    void broughtToFront() override
    {
        triggerAsyncUpdate();
    }
private:
    DragPoint attackEnd;
    std::unique_ptr<DragPointAttachment> attackMsAttach;

    DragPoint attackCurve;
    std::unique_ptr<DragPointAttachment> attackCurveAttach;

    DragPoint holdEnd;
    std::unique_ptr<DragPointAttachment> holdMsAttach;

    DragPoint decayCurve;
    std::unique_ptr<DragPointAttachment> decayCurveAttach;

    DragPoint decayEnd;
    std::unique_ptr<DragPointAttachment> sustainAttach1;
    std::unique_ptr<DragPointAttachment> decayMsAttach;

    DragPoint sustainEnd;
    std::unique_ptr<DragPointAttachment> sustainAttach2;
    std::unique_ptr<DragPointAttachment> releaseMsAttach;

    DragPoint releaseCurve;
    std::unique_ptr<DragPointAttachment> releaseCurveAttach;

    DragPoint* selectedPoint;
    bool isMoving;
    DragPoint* getPointWithinRadius(const MouseEvent& e, float radius);
    // useful for quickly iterating through points
    const std::vector<DragPoint*> points = { &attackEnd, &attackCurve, &holdEnd, &decayEnd, &decayCurve, &sustainEnd, &releaseCurve };
    // these get called from the attachment callback
    Point<float> getPosFromParam(const String& paramID, DragPoint* point, float value);
    float getParamFromPos(const String& paramID, DragPoint* point, Point<float> pos);
    Point<float> constrainPositionFor(DragPoint* point, Point<float> pos);
    //helper function, get the max limits within this component of a given DragPoint
    Rectangle<float> getLimitsFor(DragPoint* pt);
    //this grips values from the APVTS and makes sure the points match them
    void syncWithState();
    // helper for drawing handles
    static void drawHandle(Graphics& g, Point<float> center, float radius, bool fill);

};
