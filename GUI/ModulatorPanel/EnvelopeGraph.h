#pragma once
#include "../../Parameters/ElectrumValueTree.h"
#include "../Color.h"
#include "../Utility/DragPoint.h"

class EnvelopeGraph : 
public Component,
public AsyncUpdater
{
    static void drawEnvelopeGraph(AHDSRData* env, Rectangle<float>& bounds, Graphics& g);
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
    DragPoint holdEnd;
    DragPoint decayEnd;
    DragPoint sustainEnd;
    // this is responsible for gripping the data from the shared state and setting the points appropriately
    void updateDragPointPositions();
};
