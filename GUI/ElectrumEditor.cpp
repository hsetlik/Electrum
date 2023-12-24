#include "Color.h"
#include "ElectrumEditor.h"

ElectrumEditor::ElectrumEditor(EVT *tree)
    : state(tree), macro(tree), envPanel(tree), procPanel(tree), headerPanel(tree)
{
  setLookAndFeel(&lnf);
  for (int i = 0; i < NUM_OSCILLATORS; ++i)
  {
    oscEditors.add(new OscillatorEditor(state, i));
    addAndMakeVisible(oscEditors.getLast());
    oscEditors.getLast()->setLookAndFeel(&lnf);
  }
  addAndMakeVisible(macro);
  addAndMakeVisible(envPanel);
  addAndMakeVisible(procPanel);
  addAndMakeVisible(headerPanel);
}
ElectrumEditor::~ElectrumEditor() { setLookAndFeel(nullptr); }

void ElectrumEditor::paint(Graphics &g)
{
  auto fBounds = getLocalBounds().toFloat();
  auto dX = fBounds.getWidth() / 36.0f;
  auto modArea = fBounds.removeFromLeft(dX * 8.0f);

  // TODO: this is where the header and patch selector should eventually go
  auto headerArea = modArea.removeFromTop(dX * 8.0f);
  g.setColour(Color::chartreuse);
  g.fillRect(modArea);
  g.setColour(Color::mediumSeaGreen);
  g.fillRect(headerArea);
  // auto oscArea = fBounds.removeFromTop(dX * 14);
}
void ElectrumEditor::resized()
{
  auto lBounds = getLocalBounds().toFloat();
  float dX = lBounds.getWidth() / 36.0f;
  auto modArea = lBounds.removeFromLeft(dX * 8.0f);
  // TODO: this is where the header and patch selector should eventually go
  auto headerArea = modArea.removeFromTop(dX * 8.0f);
  headerPanel.setBounds(headerArea.toNearestInt());
  macro.setBounds(modArea.toNearestInt());
  auto oscArea = lBounds.removeFromTop(dX * 14);
  const float oscWidth = oscArea.getWidth() / (float)NUM_OSCILLATORS;
  // set up eac oscillator here
  for (int i = 0; i < NUM_OSCILLATORS; i++)
  {
    auto bounds = oscArea.removeFromLeft(oscWidth);
    oscEditors[i]->setBounds(bounds.toNearestInt());
  }
  // the envelope panel
  auto envArea = lBounds.removeFromLeft(12.0f * dX);
  envPanel.setBounds(envArea.toNearestInt());
  // the processor panel
  procPanel.setBounds(lBounds.toNearestInt());
}
