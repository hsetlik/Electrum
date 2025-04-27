#pragma once
#include "../Util/ScalableViewport.h"
#include "../../Shared/PointwiseWave.h"
#include "Electrum/Identifiers.h"
#include "juce_gui_basics/juce_gui_basics.h"

class ViewedPointEditor : public Component, public juce::Timer {
private:
  std::unique_ptr<Pointwise::Warp> frame;

public:
  ViewedPointEditor(const String& frameStr);
  void loadFrameString(const String& frameStr) {
    frame.reset(new Pointwise::Warp(frameStr));
    setWidthForScale(0.5f);
  }

  void resized() override;
  void paint(juce::Graphics& g) override;
  void timerCallback() override;
  void setWidthForScale(float normScale);

  void mouseDown(const juce::MouseEvent& me) override {
    frame->processMouseDown(getLocalBounds().toFloat(), me);
  }
  void mouseUp(const juce::MouseEvent& me) override {
    frame->processMouseUp(getLocalBounds().toFloat(), me);
  }
  void mouseDrag(const juce::MouseEvent& me) override {
    frame->processMouseDrag(getLocalBounds().toFloat(), me);
  }
  void mouseDoubleClick(const juce::MouseEvent& me) override {
    frame->processDoubleClick(getLocalBounds().toFloat(), me);
  }
};

//-------------------------------------------

class PointEditor : public Component {
private:
  float normScale = 0.5f;
  ViewedPointEditor editor;
  ScalableViewport vpt;

public:
  PointEditor(const String& frameStr);
  ~PointEditor() override;
  void loadFrameString(const String& frameStr);
  void resized() override { vpt.setBounds(getLocalBounds()); }
};
