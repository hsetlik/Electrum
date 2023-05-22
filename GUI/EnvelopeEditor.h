#pragma once
#include "../Parameters/ElectrumValueTree.h"
#include "Modulation/ModulationSourceComponent.h"

class EnvelopeEditor : public Component
{
private:
    EVT* const state;
    const int index;
public:
    EnvelopeEditor(EVT* tree, int idx);
    void resized() override;
};