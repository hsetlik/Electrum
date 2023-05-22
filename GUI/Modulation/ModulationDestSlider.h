#pragma once
#include "../../Parameters/ElectrumValueTree.h"


class ModulationDestSlider : public Component, public DragAndDropTarget
{
private:
    EVT* const state;
    const String paramID;
public:
    ModulationDestSlider(EVT* tree, const String& id);
    void resized() override;
    void paint(Graphics& g) override;
    void itemDropped(const juce::DragAndDropTarget::SourceDetails &dragSourceDetails) override;
    void itemDragEnter(const juce::DragAndDropTarget::SourceDetails &dragSourceDetails) override{}
    void itemDragExit(const juce::DragAndDropTarget::SourceDetails &dragSourceDetails) override{}
    void itemDragMove(const juce::DragAndDropTarget::SourceDetails &dragSourceDetails) override{}
    bool isInterestedInDragSource(const juce::DragAndDropTarget::SourceDetails &dragSourceDetails) override
    {
        return true;
    }
    bool shouldDrawDragImageWhenOver() override
    {
        return false;
    }
    void mouseDown(const juce::MouseEvent& e) override;
    Slider paramSlider; 
private:
    sAttachPtr attach;
};