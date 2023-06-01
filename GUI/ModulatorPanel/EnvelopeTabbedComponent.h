#pragma once
#include "EnvelopePanel.h"

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
};