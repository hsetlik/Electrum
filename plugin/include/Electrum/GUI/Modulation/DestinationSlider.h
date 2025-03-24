#pragma once
#include "Electrum/GUI/Modulation/WedgeButton.h"
#include "ModContextComponent.h"
class DepthSliderLookAndFeel : public juce::LookAndFeel_V4 {
public:
  void drawRotarySlider(juce::Graphics& g,
                        int x,
                        int y,
                        int width,
                        int height,
                        float sliderPosProportional,
                        float rotaryStartAngle,
                        float rotaryEndAngle,
                        juce::Slider& s) override;
};

class DepthSlider : public juce::Slider {
private:
  DepthSliderLookAndFeel lnf;

public:
  const int sourceID;
  DepthSlider(int src);
  ~DepthSlider() override;
};

//==================================================================
class ModSelectButton : public WedgeButton {
private:
  int* const selectedIdx;
  int index;

public:
  const int srcID;
  ModSelectButton(int* sel, int idx, int source);
  void paintButton(juce::Graphics& g, bool over, bool highlighted) override;
  int getIndex() { return index; }
  void setIndex(int i) { index = i; }
};

class ModCloseButton : public WedgeButton {
public:
  ModCloseButton(int destID);
  void paintButton(juce::Graphics& g, bool over, bool highlighted) override;
};

class DepthSliderStack : public juce::Component, juce::Slider::Listener {
private:
  juce::OwnedArray<DepthSlider> sliders;
  int selectedIdx = -1;

public:
  const int destID;
  DepthSliderStack(int dest);
};
//==================================================================
