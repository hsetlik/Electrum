#pragma once
#include "../../Parameters/ElectrumValueTree.h"
#include "../Modulation/ModulationSourceComponent.h"
#include "../Utility/DragPoint.h"

// define limits for where draggers go
// attack max - 2500
// hold max - 1000
// decay max - 4000
// release max - 10000
// total is 17,500- we'll call it 18,000 for simplicity and so that we always have some sustain area
#define ENV_MS_MAX 18000.0f
class EnvelopePanel : 
public Component, 
public AsyncUpdater
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
    void handleAsyncUpdate() override;
    // string helper
    String getPointName(DragPoint* pt);
    DragPointParameterAttachment* getAttachmentFor(DragPoint* pt);
private:
    DragPoint attackEnd;
    DragPoint holdEnd;
    DragPoint decayEnd;
    DragPoint sustainEnd;

    std::unique_ptr<DragPointParameterAttachment> attackMsAttach;
    std::unique_ptr<DragPointParameterAttachment> holdMsAttach;
    std::unique_ptr<DragPointParameterAttachment> decayMsAttach;
    std::unique_ptr<DragPointParameterAttachment> sustainAttach;
    std::unique_ptr<DragPointParameterAttachment> releaseMsAttach;

    const std::vector<DragPoint*> points = { &attackEnd, &holdEnd, &decayEnd, &sustainEnd };
    DragPoint* selectedPoint;
    void syncWithState();
// some static handle drawing methods to clean up paint()
    static void drawHandle(Graphics&g, Component* parent, DragPoint* point, bool shouldBeFilled);
// the callbacks lambdas that we pass to the attachments use these
// this needs to handle moving any points affected by this parameter
    void dragParamChanged(const String& id, float newValue);
    float parameterFromPoint(const String& id, float x, float y);
// this returns the first point within a given distance of the MouseEvent, or nullptr if none are within the distance
    DragPoint* getPointWithin(const MouseEvent& e, float radius);

};