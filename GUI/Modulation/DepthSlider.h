#pragma once
#include "WedgeButton.h"

class DepthSliderLookAndFeel : public LookAndFeel_V4
{
public:
    void drawRotarySlider(Graphics & g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, Slider &s) override;
};
//=============================================================================
class DepthSlider : public Component, public Slider::Listener
{
private:
    EVT* const state;
    DepthSliderLookAndFeel lnf;
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
        params.startAngleRadians = MathConstants<float>::pi * 1.35f;
        params.endAngleRadians = MathConstants<float>::pi * 2.65f;
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