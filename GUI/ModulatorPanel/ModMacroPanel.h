#pragma once
#include "../ElectrumLookAndFeel.h"
#include "LFOTabbedComponent.h"
#include "ModWheelSource.h"
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
  LFOTabbedComponent lfos;
};
