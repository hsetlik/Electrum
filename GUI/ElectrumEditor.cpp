#include "ElectrumEditor.h"

ElectrumEditor::ElectrumEditor(EVT *tree)
    : state(tree), macro(tree), envPanel(tree), procPanel(tree)
{
  setLookAndFeel(&lnf);
  for (int i = 0; i < NUM_OSCILLATORS; ++i) {
    oscEditors.add(new OscillatorEditor(state, i));
    addAndMakeVisible(oscEditors.getLast());
  }
  addAndMakeVisible(macro);
  addAndMakeVisible(envPanel);
  addAndMakeVisible(procPanel);
}
ElectrumEditor::~ElectrumEditor() { setLookAndFeel(nullptr); }

void ElectrumEditor::paint(Graphics &) {}
void ElectrumEditor::resized()
{
  auto lBounds = getLocalBounds().toFloat();
  float dX = lBounds.getWidth() / 36.0f;
  auto modArea = lBounds.removeFromLeft(dX * 8.0f);
  // TODO: this is where the header and patch selector should eventually go
  // auto headerArea = modArea.removeFromTop(dX * 8.0f);
  macro.setBounds(modArea.toNearestInt());
  auto oscArea = lBounds.removeFromTop(dX * 14);
  const float oscWidth = oscArea.getWidth() / (float)NUM_OSCILLATORS;
  // set up eac oscillator here
  for (int i = 0; i < NUM_OSCILLATORS; i++) {
    auto bounds = oscArea.removeFromLeft(oscWidth);
    oscEditors[i]->setBounds(bounds.toNearestInt());
  }
  // the envelope panel
  auto envArea = lBounds.removeFromLeft(12.0f * dX);
  envPanel.setBounds(envArea.toNearestInt());
  // the processor panel
  procPanel.setBounds(lBounds.toNearestInt());
}
