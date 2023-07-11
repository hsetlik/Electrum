#pragma once
#include "../../Parameters/ElectrumValueTree.h"
#include "../Color.h"
#include "../Modulation/ModulationDestSlider.h"

class FilterPanel : public Component
{
public:
    EVT* const state;
    FilterPanel(EVT* tree);
    void resized() override;
private:
    std::unique_ptr<ModulationDestSlider> sCutoff;
    std::unique_ptr<ModulationDestSlider> sRes;
    std::unique_ptr<ModulationDestSlider> sMix;
    std::unique_ptr<ModulationDestSlider> sTracking;
    ComboBox bFilterType;
    std::unique_ptr<APVTS::ComboBoxAttachment> typeAttach;
};
