#pragma once

#include "Electrum/GUI/LookAndFeel/BinaryGraphics.h"
class PowerButton : public juce::Button {
public:
  PowerButton() : juce::Button("pwrButton") { setClickingTogglesState(true); }
  void paintButton(juce::Graphics& g, bool, bool) override {
    auto fBounds = getLocalBounds().toFloat();
    ImgData::ImgID id =
        getToggleState() ? ImgData::ImgID::PowerOn : ImgData::ImgID::PowerOff;
    auto width = ImgData::widthForBounds(fBounds);
    auto& img = ImgData::getBinaryImage(id, width);
    jassert(img.isValid());
    g.drawImage(img, fBounds);
  }
};
