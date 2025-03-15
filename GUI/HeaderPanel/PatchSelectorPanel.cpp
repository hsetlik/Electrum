#include "PatchSelectorPanel.h"
#include "juce_core/juce_core.h"

PatchSelectorPanel::PatchSelectorPanel(EVT *tree, Component *loadComp, Component *saveComp)
    : state(tree), openDialog(loadComp), saveDialog(saveComp), bOpen(VectorSymbols::Open),
      bSave(VectorSymbols::Save)
{
  juce::ignoreUnused(state);
  addAndMakeVisible(&bOpen);
  addAndMakeVisible(&bSave);

  // callbacks for the buttons
  bOpen.onClick = [this]() {
    openDialog->setVisible(true);
    openDialog->setEnabled(true);
    openDialog->getParentComponent()->resized();
  };

  bSave.onClick = [this]() {
    saveDialog->setVisible(true);
    saveDialog->setEnabled(true);
    saveDialog->getParentComponent()->resized();
  };
}

void PatchSelectorPanel::resized()
{
  auto fBounds = getLocalBounds().toFloat();
  float buttonSide = std::min(fBounds.getHeight(), fBounds.getWidth() / 8.0f);
  auto oBounds = fBounds.removeFromLeft(buttonSide).toNearestInt();
  auto sBounds = fBounds.removeFromLeft(buttonSide).toNearestInt();
  bOpen.setBounds(oBounds);
  bSave.setBounds(sBounds);
}
