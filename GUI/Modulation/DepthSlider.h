#pragma once
#include "../../Parameters/ElectrumValueTree.h"

class DepthSlider : public Component, public Slider::Listener
{
private:
    EVT* const state;
public:
    const String sourceID;
    const String destID;
private:
    int destIndex;
    Slider slider;
    float lastSliderValue;
    // helper for how rot. parameters are stored
    static Slider::RotaryParameters getDepthSliderParams()
    {
        Slider::RotaryParameters params;
        params.startAngleRadians = 0.0f; 
        params.endAngleRadians = MathConstants<float>::pi * 1.5f;
        params.stopAtEnd = true;
        return params;
    }
public:
    DepthSlider(EVT* state, const String& src, const String& dst, int idx);
    ~DepthSlider() override;
    //slider callback
    void sliderValueChanged(Slider* s) override;
    void resized() override;
    void setIndex(int idx) { destIndex = idx; }    
    int getIndex() const { return destIndex; }
};