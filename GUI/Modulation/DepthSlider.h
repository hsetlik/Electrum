#pragma once
#include "../../Parameters/ElectrumValueTree.h"
#include "../Color.h"

class DepthSliderLookAndFeel : public LookAndFeel_V4
{
private:
    const String sliderThumbSVG = "m3.64.82L.04,13.5c-.2.72.34,1.44,1.09,1.44h7.2c.75,0,1.29-.72,1.09-1.44L5.81.82c-.31-1.09-1.86-1.09-2.17,0Z";
public:
    void drawRotarySlider(Graphics & g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, Slider &s) override;
};
//=============================================================================
class CloseButton : public Button
{
private:
    static Path getXPath()
    {
        return Drawable::parseSVGPath("m13.94,11.34l-3.92-3.92c-.2-.2-.2-.53,0-.74l3.92-3.92c.2-.2.2-.53,0-.74l-1.86-1.86c-.2-.2-.53-.2-.74,0l-3.92,3.92c-.2.2-.53.2-.74,0L2.75.15c-.2-.2-.53-.2-.74,0L.15,2.01c-.2.2-.2.53,0,.74l3.92,3.92c.2.2.2.53,0,.74L.15,11.34c-.2.2-.2.53,0,.74l1.86,1.86c.2.2.53.2.74,0l3.92-3.92c.2-.2.53-.2.74,0l3.92,3.92c.2.2.53.2.74,0l1.86-1.86c.2-.2.2-.53,0-.74Z");
    }
    
public:
    const String sourceID;
    const String destID;
    CloseButton(const String& srcID, const String& dstID) :
    Button(srcID + "to" + dstID + "Remove"),
    sourceID(srcID),
    destID(dstID),
    xPath(getXPath())
    {
    }
    void paintButton(Graphics& g, bool highlighted, bool down) override;
private:
    Path xPath;
};
//=============================================================================
class ModSelectButton : public Button
{
public:
    const String destID; 
    const String sourceID; 
    ModSelectButton(const String& dstID, const String& srcID) :
    Button(srcID + "to" + dstID + "Select"),
    destID(dstID),
    sourceID(srcID)
    {
    }
    void paintButton(Graphics& g, bool highlighted, bool down) override;
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
        params.startAngleRadians = MathConstants<float>::pi * 1.25f;
        params.endAngleRadians = MathConstants<float>::pi * 2.75f;
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