#pragma once
#include "EnvelopePanel.h"
#include "../Modulation/ModulationSourceComponent.h"

class EnvelopeSourceComponent : public Component
{
public:
    EnvelopeSourceComponent(EVT* tree);
    void resized() override;
private:
    EVT* const state;
    OwnedArray<ModulationSourceComponent> sources;
};
//==================================================
class EnvelopeTabbedComponent : public Component
{
private:
    OwnedArray<TextButton> buttons;
    OwnedArray<EnvelopePanel> envPanels;
    int selectedEnvIndex;
    void setSelectedIndex(int idx)
    {
        selectedEnvIndex = idx;
        resized();
    }
public:
    EVT* const state;
    EnvelopeTabbedComponent(EVT* tree);
    void resized() override;
private:
    EnvelopeSourceComponent sources;
};