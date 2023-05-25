#pragma once
#include "../../Parameters/ElectrumValueTree.h"
#include "../Modulation/ModulationSourceComponent.h"
#include "PerlinGraph.h"
class PerlinPanel : public Component
{
private:
    EVT* const state;
    Slider sFreq;
    Slider sLac;
    Slider sOct;
    PerlinGraph graph;

    sAttachPtr freqAttach;
    sAttachPtr lacAttach;
    sAttachPtr octAttach;
public:
    PerlinPanel(EVT* tree);
    void resized() override;
    void paint(Graphics& g) override;


};