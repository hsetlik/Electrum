#pragma once
#include "WedgeButton.h"

class DepthSliderLookAndFeel : public LookAndFeel_V4
{
private:
    const String sliderThumbSVG = "m3.64.82L.04,13.5c-.2.72.34,1.44,1.09,1.44h7.2c.75,0,1.29-.72,1.09-1.44L5.81.82c-.31-1.09-1.86-1.09-2.17,0Z";
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