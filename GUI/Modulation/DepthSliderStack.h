#pragma once
#include "DepthSlider.h"
#include "../Utility/WedgeButton.h"
//Special buttons for the mod. parameters
class ModSelectButton : public WedgeButton
{
private:
    int* const selectedSrcIdx;
    int index;
public:
    const String sourceID;
    ModSelectButton(int& selected, int idx, const String& srcName);
    void paintButton(Graphics& g, bool isOver, bool isHighlighted);

};
//==================================================================
class DepthSliderStack : public Component
{
private:
    EVT* const state;
    OwnedArray<DepthSlider> sliders;
    OwnedArray<ModSelectButton> selectButtons;
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