#pragma once
#include "DepthSlider.h"
#include "WedgeButton.h"

class ModSelectButton : public WedgeButton
{
private:
    int* const selectedIdx;
    int index;
public:
    const String sourceID;
    ModSelectButton(int& sel, int idx, const String& srcID);
    void paintButton(Graphics& g, bool over, bool highlighted) override;
    int getIndex() { return index; }
    void setIndex(int i) { index = i; }
};

//==================================================================
class ModCloseButton : public WedgeButton
{
public:
    ModCloseButton(const String& destID);
    void paintButton(Graphics& g, bool over, bool highlighted) override;
};
//==================================================================
class DepthSliderStack : public Component
{
private:
    EVT* const state;
    OwnedArray<DepthSlider> sliders;
    OwnedArray<ModSelectButton> selectButtons;
    ModCloseButton closeButton;
    void removeSelectedSource();
    void selectSource(const String& sourceID);
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