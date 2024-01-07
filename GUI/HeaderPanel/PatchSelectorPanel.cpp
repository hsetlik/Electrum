#include "PatchSelectorPanel.h"

PatchSelectorPanel::PatchSelectorPanel(EVT *tree)
    : state(tree), bOpen(VectorSymbols::Open), bSave(VectorSymbols::Save)
{
  addAndMakeVisible(&bOpen);
  addAndMakeVisible(&bSave);
  // TODO: lambdas for the button callbacks go here
}
