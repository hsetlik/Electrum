#pragma once

#include "ImgData.h"
#include "juce_graphics/juce_graphics.h"
#include "juce_gui_basics/juce_gui_basics.h"

namespace ImgData {
enum ImgID { Expand, ExpandHL };
enum ImgWidth { p256, p128, p64 };

juce::Image& getBinaryImage(ImgID id, ImgWidth iwdth);
ImgWidth widthForBounds(const juce::Rectangle<float>& bounds);

}  // namespace ImgData

enum button_image_t { ExpandBtn };
class ImgButton : public juce::Button {
  button_image_t imgType;

public:
  ImgButton(button_image_t t = button_image_t::ExpandBtn);
  void paintButton(juce::Graphics& g, bool highlighted, bool down) override;
};
