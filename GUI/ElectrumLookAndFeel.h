#pragma once
#include "Color.h"
#include "Fonts.h"
#ifndef ELECTRUMLOOKANDFEEL_H
#define ELECTRUMLOOKANDFEEL_H
#define LABEL_DEFAULT_FONT_HEIGHT 15.0f
class ElectrumLookAndFeel : public LookAndFeel_V4
{
public:
  ElectrumLookAndFeel();
  void drawRotarySlider(Graphics &g, int x, int y, int width, int height,
                        float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle,
                        Slider &) override;
  Font getLabelFont(Label &) override;
  void drawLabel(Graphics &g, Label &l) override;

private:
  // store these so we don't have to reload the ttf file every time we draw text
  Font labelFont;
};
#endif
