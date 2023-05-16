#pragma once
#include "../Parameters/ElectrumValueTree.h"
#include "OscillatorEditor.h"

class ElectrumEditor : public Component
{
private:
    EVT* const state;
    OwnedArray<OscillatorEditor> oscEditors;
public:
    ElectrumEditor(EVT* tree);
    ~ElectrumEditor() override;
    void paint(Graphics& g) override;
    void resized() override;
};