#pragma once
#include "DepthSlider.h"

class DepthSliderStack : public Component
{
private:
    EVT* const state;
    OwnedArray<DepthSlider> sliders;
    void reindexSliders();
public:
    const String destID;
private:
    int selectedSliderIndex;
public:
    DepthSliderStack(EVT* tree, const String& dst);
    bool hasModulationFrom(const String& srcID);
    void addModulationFor(const String& srcID);
    void removeModulationFrom(const String& srcID);
    void resized() override;
    void paint(Graphics& g) override;
};