#pragma once
#include "DepthSlider.h"
#include "../Utility/WedgeButton.h"
//Special buttons for the mod. parameters
class ModSelectButton : public WedgeButton
{
private:
    OwnedArray<ModSelectButton>* const allButtons;
public:
    const String sourceID;
    ModSelectButton(OwnedArray<ModSelectButton>* arr, const String& srcName);
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