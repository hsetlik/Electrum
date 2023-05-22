#pragma once
#include "../Parameters/ElectrumValueTree.h"
#include "GeneratorPanel/OscillatorEditor.h"
#include "ModulatorPanel/ModWheelSource.h"

class ElectrumEditor : 
public Component, 
public DragAndDropContainer
{
private:
    EVT* const state;
    OwnedArray<OscillatorEditor> oscEditors;
    ModWheelSource modWhlSource;
public:
    ElectrumEditor(EVT* tree);
    ~ElectrumEditor() override;
    void paint(Graphics& g) override;
    void resized() override;
};