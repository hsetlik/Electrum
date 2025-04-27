#pragma once

#include "juce_gui_basics/juce_gui_basics.h"
typedef std::function<void(float)> norm_scale_callback;

class ScalableViewport : public juce::Viewport {
private:
  norm_scale_callback scaleCallback;

  juce::Slider scaleSlider;

public:
  ScalableViewport();
  void setScaleCallback(const norm_scale_callback& cb);
  void resized() override;
  void paint(juce::Graphics& g) override;
};
