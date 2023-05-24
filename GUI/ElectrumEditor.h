#pragma once
#include "../Parameters/ElectrumValueTree.h"
#include "GeneratorPanel/OscillatorEditor.h"
#include "ModulatorPanel/ModWheelSource.h"
#include "ModulatorPanel/PitchWheelSource.h"
#include "ModulatorPanel/PerlinPanel.h"
#include "ElectrumLookAndFeel.h"

class ElectrumEditor : 
public Component, 
public DragAndDropContainer
{
private:
    EVT* const state;
    OwnedArray<OscillatorEditor> oscEditors;
    ModWheelSource modWhlSource;
    PitchWheelSource pitchWhlSource;
    ElectrumLookAndFeel lnf;
    PerlinPanel perlin;
public:
    ElectrumEditor(EVT* tree);
    ~ElectrumEditor() override;
    void paint(Graphics& g) override;
    void resized() override;
};