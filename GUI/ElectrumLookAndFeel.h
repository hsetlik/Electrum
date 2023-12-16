#pragma once
#include "Color.h"
#include "Fonts.h"
#ifndef ELECTRUMLOOKANDFEEL_H
#define ELECTRUMLOOKANDFEEL_H
class ElectrumLookAndFeel : public LookAndFeel_V4
{
public:
  ElectrumLookAndFeel();
  void drawRotarySlider(Graphics &g, int x, int y, int width, int height,
                        float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle,
                        Slider &) override;
};
#endif
