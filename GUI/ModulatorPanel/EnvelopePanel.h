#pragma once
#include "EnvelopeGraph.h"
#include "../../Parameters/ElectrumValueTree.h"
#define ENV_LABEL_HEIGHT 15
class EnvelopePanel : public Component
{
private:
    EVT* const state;
    EnvelopeGraph graph;
public:
    const int index;
    EnvelopePanel(EVT* tree, int idx);
    void resized() override;

};