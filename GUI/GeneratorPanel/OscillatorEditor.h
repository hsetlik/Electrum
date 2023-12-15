#pragma once
#include "../../Parameters/Identifiers.h"
#include "../ElectrumLookAndFeel.h"
#include "../Modulation/LabeledMDS.h"
#include "WavetableGraph.h"
#include <FontBinaries.h>

class OscillatorEditor : public Component, public Timer
{
private:
  EVT *state;
  const int index;
  LabeledMDS sLevel;
  LabeledMDS sPos;
  LabeledMDS sPan;
  LabeledMDS sCoarse;
  LabeledMDS sFine;
  // TODO: replace this with just drawing the text
  Label label;
  WavetableGraph graph;
  Typeface::Ptr noodle;

public:
  OscillatorEditor(EVT *tree, int idx);
  ~OscillatorEditor() override;
  void resized() override;
  void paint(Graphics &g) override;
  void timerCallback() override;
};
