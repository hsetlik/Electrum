#pragma once
#include "../../Parameters/ElectrumValueTree.h"
#include "../Modulation/ModulationSourceComponent.h"
#define METER_WIDTH 0.0675f
#define METER_REFRESH_HZ 24

class PerlinPanel : public Component, public Timer
{
private:
    EVT* const state;
    Slider sFreq;
    Slider sLac;
    Slider sOct;

    sAttachPtr freqAttach;
    sAttachPtr lacAttach;
    sAttachPtr octAttach;
public:
    PerlinPanel(EVT* tree);
    void resized() override;
    void paint(Graphics& g) override;
    //timer
    void timerCallback() override
    {
        repaint();
    }


};