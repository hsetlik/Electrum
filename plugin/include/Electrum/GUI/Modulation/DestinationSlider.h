#pragma once
#include "Electrum/GUI/Modulation/WedgeButton.h"
#include "Electrum/Shared/ElectrumState.h"
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
  int* selectedSrc;
  int index;

public:
  const int srcID;
  ModSelectButton(int* sel, int idx, int source);
  ~ModSelectButton() override;
  void paintButton(juce::Graphics& g, bool over, bool highlighted) override;
  int getIndex() { return index; }
  void setIndex(int i) { index = i; }
};

class ModCloseButton : public WedgeButton {
public:
  ModCloseButton() : WedgeButton("closeBtn") {}
  void paintButton(juce::Graphics& g, bool over, bool highlighted) override;
};

class DepthSliderStack : public juce::Component, juce::Slider::Listener {
private:
  juce::OwnedArray<DepthSlider> sliders;
  juce::OwnedArray<ModSelectButton> selectButtons;
  ModCloseButton closeButton;
  int selectedSrc = -1;
  ElectrumState* const state;
  DepthSlider* selectedSlider = nullptr;
  // checks whether the state of this component is in line
  // with the master state ValueTree
  bool _trySimpleConform();
  void _setSliderForSrc(int src, float value);
  void _reindexButtons();
  void _setSelectedDepthSlider(int srcID);
  void _selectSource(int src);
  // helper helpers
  DepthSlider* _sliderForSrc(int src);
  ModSelectButton* _btnForSource(int src);

public:
  const int destID;
  DepthSliderStack(ElectrumState* s, int dest);
  // the drag & drop interface interacts with these
  bool hasComponentsForSrc(int src);
  void addModulation(int src);
  void removeModulation(int src);
  void sliderValueChanged(juce::Slider* s) override;
  void resized() override;
  void reinitFromState();
};
//==================================================================
