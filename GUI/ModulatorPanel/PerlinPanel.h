#pragma once
#include "../../Parameters/ElectrumValueTree.h"
#include "../Modulation/ModulationSourceComponent.h"
#include "../Modulation/SourceParamSlider.h"
#include "PerlinGraph.h"
class PerlinPanel : public Component
{
private:
    EVT* const state;
    SourceParamSlider sFreq;
    SourceParamSlider sLac;
    SourceParamSlider sOct;
    PerlinGraph graph;

public:
    PerlinPanel(EVT* tree);
    void resized() override;
    void paint(Graphics& g) override;


};