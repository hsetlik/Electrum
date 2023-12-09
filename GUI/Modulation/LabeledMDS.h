#pragma once
#include "../Color.h"
#include "ModulationDestSlider.h"
#include <FontBinaries.h>

class LabeledMDS : public Component
{
public:
  LabeledMDS(EVT *tree, const String &paramID, const String &labelText);
  void resized() override;
  void setTextColor(const Colour &c) { textColor = c; }

private:
  std::unique_ptr<ModulationDestSlider> mds;
  Label label;
  const String text;
  Font labelFont;
  Colour textColor;
};
