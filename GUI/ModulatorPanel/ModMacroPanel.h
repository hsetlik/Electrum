#pragma once
#include "../ElectrumLookAndFeel.h"
#include "ModWheelSource.h"
#include "PerlinPanel.h"
#include "PitchWheelSource.h"

class ModMacroPanel : public Component
{
public:
  ModMacroPanel(EVT *tree);
  ~ModMacroPanel() override;
  void paint(Graphics &g) override;
  void resized() override;

private:
  PitchWheelSource pitch;
  ModWheelSource mod;
  PerlinPanel perlin;
};
