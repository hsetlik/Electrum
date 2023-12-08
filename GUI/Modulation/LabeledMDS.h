#pragma once
#include "../Color.h"
#include "ModulationDestSlider.h"
#include <FontBinaries.h>

class LabeledMDS : public Component
{
public:
  LabeledMDS(EVT *tree, const String &paramID, const String &labelText);
  void resized() override;
  void paint(Graphics &g) override;

private:
  std::unique_ptr<ModulationDestSlider> mds;
  const String text;
  Font labelFont;
};
