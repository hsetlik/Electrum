#pragma once
#include "../../Parameters/ElectrumValueTree.h"
#include "../Modulation/LabeledSPS.h"
#include "../Modulation/ModulationSourceComponent.h"
#include "PerlinGraph.h"
class PerlinPanel : public Component
{
private:
  EVT *const state;
  LabeledSPS sFreq;
  LabeledSPS sLac;
  LabeledSPS sOct;
  PerlinGraph graph;
  ModulationSourceComponent sComp;

public:
  PerlinPanel(EVT *tree);
  void resized() override;
  void paint(Graphics &g) override;
};
