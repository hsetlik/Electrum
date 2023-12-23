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
  // label stuff
  Font getLabelFont(Label &) override;
  void drawLabel(Graphics &g, Label &l) override;
  // tab button bar stuff
  void drawTabButton(TabBarButton &, Graphics &, bool isMouseOver, bool isMouseDown) override;
  Font getTabButtonFont(TabBarButton &, float height) override;
  void drawTabButtonText(TabBarButton &b, Graphics &g, bool isMouseOver, bool isMouseDown) override;
  void createTabButtonShape(TabBarButton &, Path &path, bool isMouseOver,
                            bool isMouseDown) override;
  int getTabButtonBestWidth(TabBarButton &button, int tabDepth) override;
  void fillTabButtonShape(TabBarButton &button, Graphics &g, const Path &path, bool isMouseOver,
                          bool isMouseDown) override;
  void drawTabbedButtonBarBackground(TabbedButtonBar &bar, Graphics &g) override;
  void drawTabAreaBehindFrontButton(TabbedButtonBar &, Graphics &, int w, int h) override;
  // combo box stuff
  void drawComboBox(Graphics &g, int width, int height, bool isButtonDown, int buttonX, int buttonY,
                    int buttonW, int buttonH, ComboBox &box) override;
  Font getComboBoxFont(ComboBox &) override;
  Label *createComboBoxTextBox(ComboBox &) override;
  void positionComboBoxText(ComboBox &box, Label &labelToPosition) override;

private:
  // store these so we don't have to reload the ttf file every time we draw text
  Font labelFont;
  Font tabButtonFont;
  Font comboBoxFont;
};
#endif
