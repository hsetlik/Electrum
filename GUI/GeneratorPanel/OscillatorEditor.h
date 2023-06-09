#pragma once
#include "../Modulation/ModulationDestSlider.h"
#include "../../Parameters/Identifiers.h"
#include "WavetableGraph.h"

class OscillatorEditor : public Component
{
private:
    EVT* state;
    const int index;
    //std::unique_ptr<ModulationDestSlider> sPos;
    Label label;
    ModulationDestSlider sLevel;
    ModulationDestSlider sPos;
    WavetableGraph graph;

public:
    OscillatorEditor(EVT* tree, int idx);
    void resized() override;
    void paint(Graphics& g) override;
};