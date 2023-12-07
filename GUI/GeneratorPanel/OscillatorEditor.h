#pragma once
#include "../../Parameters/Identifiers.h"
#include "../Modulation/ModulationDestSlider.h"
#include "WavetableGraph.h"

class OscillatorEditor : public Component
{
private:
  EVT *state;
  const int index;
  std::unique_ptr<ModulationDestSlider> sLevel;
  std::unique_ptr<ModulationDestSlider> sPos;
  std::unique_ptr<ModulationDestSlider> sCoarse;
  std::unique_ptr<ModulationDestSlider> sFine;
  Label label;
  WavetableGraph graph;

public:
  OscillatorEditor(EVT *tree, int idx);
  void resized() override;
  void paint(Graphics &g) override;
};
