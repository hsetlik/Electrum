#pragma once
#include "../../Parameters/ElectrumValueTree.h"
#include "../Color.h"
#include "../Utility/DragPoint.h"

class EnvelopeGraph : 
public Component,
public AsyncUpdater
{
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
    // useful for quickly iterating through points
    const std::vector<DragPoint*> points = { &attackEnd, &holdEnd, &decayEnd, &sustainEnd };
    // this is responsible for gripping the data from the shared state and setting the points appropriately
    void updateDragPointPositions();
    // these get called from the attachment callback
    Point<float> getPosFromParam(const String& paramID, DragPoint* point, float value);
    float getParamFromPos(const String& paramID, DragPoint* point, Point<float> pos);
    Point<float> constrainPositionFor(DragPoint* point, Point<float> pos);

};
