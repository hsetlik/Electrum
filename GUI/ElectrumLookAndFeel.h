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
  // tab button bar stuff
  void drawTabButton(TabBarButton &, Graphics &, bool isMouseOver, bool isMouseDown) override;
  Font getTabButtonFont(TabBarButton &, float height) override;
  void drawTabButtonText(TabBarButton &b, Graphics &g, bool isMouseOver, bool isMouseDown) override;
  void createTabButtonShape(TabBarButton &, Path &path, bool isMouseOver,
                            bool isMouseDown) override;

private:
  // store these so we don't have to reload the ttf file every time we draw text
  Font labelFont;
  Font tabButtonFont;
};
#endif
