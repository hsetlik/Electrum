#pragma once
#include "../Modulation/ModulationSourceComponent.h"
#include "../../Parameters/ElectrumValueTree.h"
class ModWheelSource : public Component
{
private:
    EVT* const state;
    Label label;
    ModulationSourceComponent sourceComp;
public:
    ModWheelSource(EVT* tree) : 
    state(tree),
    label("mod_wheel_label", "Mod Wheel"),
    sourceComp(tree, IDs::modWheelSource.toString())
    {
        addAndMakeVisible(label);
        addAndMakeVisible(sourceComp);
    }

    void resized() override
    {
        auto lBounds = getLocalBounds().toFloat();
        label.setBounds(lBounds.removeFromTop(17.0f).toNearestInt());
        sourceComp.setBounds(lBounds.toNearestInt());

    }
};
