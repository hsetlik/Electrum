#pragma once
#include "../../Parameters/ElectrumValueTree.h"
#include "../Modulation/ModulationSourceComponent.h"

// top-level component that gets added to the ElectrumEditor
enum DragPointID
{
    Peak,
    Sustain,
    Release
};


class EnvelopePanel : public Component
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


};