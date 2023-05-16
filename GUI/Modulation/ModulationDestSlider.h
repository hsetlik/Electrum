#pragma once
#include "../../Parameters/ElectrumValueTree.h"


class ModulationDestSlider : public Component
{
private:
    EVT* const state;
    const String paramID;
    //sAttachPtr attach;
public:
    ModulationDestSlider(EVT* tree, const String& id);
    void resized() override;
    void paint(Graphics& g) override;
    Slider paramSlider; 
};