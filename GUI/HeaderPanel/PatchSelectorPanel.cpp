#include "PatchSelectorPanel.h"

PatchSelectorPanel::PatchSelectorPanel(EVT *tree)
    : state(tree), bOpen(VectorSymbols::Open), bSave(VectorSymbols::Save)
{
  addAndMakeVisible(&bOpen);
  addAndMakeVisible(&bSave);
  // TODO: lambdas for the button callbacks go here
}

void PatchSelectorPanel::resized()
{
  auto fBounds = getLocalBounds().toFloat();
  float buttonSide = std::min({fBounds.getHeight(), fBounds.getWidth() / 10.0f});
  auto oBounds = fBounds.removeFromLeft(buttonSide).toNearestInt();
  auto sBounds = fBounds.removeFromLeft(buttonSide).toNearestInt();
  bOpen.setBounds(oBounds);
  bSave.setBounds(sBounds);
}
