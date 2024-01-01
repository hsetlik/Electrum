#pragma once
#include "../../Parameters/Identifiers.h"
#include "../ElectrumLookAndFeel.h"
#include "../Modulation/LabeledMDS.h"
#include "BitmapWavetableGraph.h"
#include "WavetableGraph.h"
#include <FontBinaries.h>

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
  BitmapWavetableGraph graph;

public:
  OscillatorEditor(EVT *tree, int idx);
  ~OscillatorEditor() override;
  void resized() override;
  void paint(Graphics &g) override;
};
