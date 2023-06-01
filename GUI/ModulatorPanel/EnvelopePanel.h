#pragma once
#include "../../Parameters/ElectrumValueTree.h"
#include "../Modulation/ModulationSourceComponent.h"
#include "../Utility/DragPoint.h"

// define limits for where draggers go
// attack max - 2500
// hold max - 1000
// decay max - 4000
// release max - 10000
// total is 17,500
class EnvelopePanel : public Component, public DragPoint::Listener
{
public:
    EVT* const state;
    const int index;
    EnvelopePanel(EVT* tree, int idx);

    void paint(Graphics& g) override;
    //mouse callbacks
    void mouseDown(const MouseEvent &event) override;
    void mouseDrag(const MouseEvent &event) override;
    void mouseUp(const MouseEvent &event) override;
    // listener
    void pointMoved(DragPoint* point, float newX, float newY) override;
    
private:
    DragPoint attackEnd;
    DragPoint holdEnd;
    DragPoint decayEnd;
    DragPoint sustainEnd;
    const std::vector<DragPoint*> points = { &attackEnd, &holdEnd, &decayEnd, &sustainEnd };
    DragPoint* selectedPoint;
    void syncWithState();
// some static handle drawing methods to clean up paint()
    static void drawHandle(Graphics&g, Component* parent, DragPoint* point, bool shouldBeFilled);

};