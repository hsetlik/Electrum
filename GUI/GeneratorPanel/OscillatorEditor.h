#pragma once
#include "../../Parameters/Identifiers.h"
#include "../Modulation/LabeledMDS.h"
#include "WavetableGraph.h"

class OscillatorEditor : public Component
{
private:
  EVT *state;
  const int index;
  LabeledMDS sLevel;
  LabeledMDS sPos;
  LabeledMDS sPan;
  LabeledMDS sCoarse;
  LabeledMDS sFine;
  Label label;
  WavetableGraph graph;

public:
  OscillatorEditor(EVT *tree, int idx);
  void resized() override;
  void paint(Graphics &g) override;
};
