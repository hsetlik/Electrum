#pragma once
#include "EnvelopeGraph.h"
#include "../../Parameters/ElectrumValueTree.h"

class EnvelopePanel : Component
{
private:
    EVT* const state;
public:
    const int index;
    EnvelopePanel(EVT* tree, int idx);
    void resized() override;
    void paint(Graphics& g) override;

};