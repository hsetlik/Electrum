#pragma once
#include "../../Parameters/ElectrumValueTree.h"
#include "../Color.h"
#include "../Utility/DragPoint.h"

class EnvelopeGraph : 
public Component,
public AsyncUpdater
{
    // just some static helpers to figure things out. . .
    static float maxAttackLength(Rectangle<float>& bounds)
    {
        float t = (float)ATTACK_MS_MAX / (float)ENV_MS_MAX;
        return t * bounds.getWidth();
    }
    static float maxHoldLength(Rectangle<float>& bounds)
    {
        float t = (float)HOLD_MS_MAX / (float)ENV_MS_MAX;
        return t * bounds.getWidth();
    }
    static float maxDecayLength(Rectangle<float>& bounds)
    {
        float t = (float)DECAY_MS_MAX / (float)ENV_MS_MAX;
        return t * bounds.getWidth();
    }
    static float maxReleaseLength(Rectangle<float>& bounds)
    {
        float t = (float)RELEASE_MS_MAX / (float)ENV_MS_MAX;
        return t * bounds.getWidth();
    }
    static float sustainLevelY(Rectangle<float>& bounds, float val)
    {
        float min = bounds.getY() + 5.0f;
        float max = bounds.getBottom() - 5.0f;
        return jmap((1.0f - val), 0.0f, 1.0f, min, max);
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
private:
    DragPoint attackEnd;
    std::unique_ptr<DragPointAttachment> attackAttach;
    DragPoint holdEnd;
    std::unique_ptr<DragPointAttachment> holdAttach;
    DragPoint decayEnd;
    std::unique_ptr<DragPointAttachment> sustainAttach1;
    std::unique_ptr<DragPointAttachment> decayAttach;
    DragPoint sustainEnd;
    std::unique_ptr<DragPointAttachment> sustainAttach2;
    std::unique_ptr<DragPointAttachment> releaseAttach;
    DragPoint* selectedPoint;
    bool isMoving;
    DragPoint* getPointWithinRadius(const MouseEvent& e, float radius);
    // useful for quickly iterating through points
    const std::vector<DragPoint*> points = { &attackEnd, &holdEnd, &decayEnd, &sustainEnd };
    // this is responsible for gripping the data from the shared state and setting the points appropriately
    void updateDragPointPositions();
    // these get called from the attachment callback
    Point<float> getPosFromParam(const String& paramID, DragPoint* point, float value);
    float getParamFromPos(const String& paramID, DragPoint* point, Point<float> pos);
    Point<float> constrainPositionFor(DragPoint* point, Point<float> pos);
    //helper function, get the max limits within this component of a given DragPoint
    Rectangle<float> getLimitsFor(DragPoint* pt);

};
