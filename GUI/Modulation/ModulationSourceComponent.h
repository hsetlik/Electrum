#pragma once
#include "../../Parameters/ElectrumValueTree.h"

class ModulationSourceComponent : public Component
{
private:
    EVT* const state;
public:
    const String sourceID;
    ModulationSourceComponent(EVT* tree, const String& id);
    void paint(Graphics& g) override;
    void mouseDown(const juce::MouseEvent& e) override;
};