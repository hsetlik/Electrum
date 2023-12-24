#include "ModMacroPanel.h"

ModMacroPanel::ModMacroPanel(EVT *tree) : pitch(tree), mod(tree), lfos(tree)
{
  addAndMakeVisible(&pitch);
  addAndMakeVisible(&mod);
  addAndMakeVisible(&lfos);
}
ModMacroPanel::~ModMacroPanel() {}
void ModMacroPanel::paint(Graphics &g)
{
  auto b = getLocalBounds().toFloat();
  g.setColour(Colours::black);
  g.fillRect(b);
}
void ModMacroPanel::resized()
{
  // the aspect ratio of this component is fixed at 1:2
  auto lBounds = getLocalBounds().toFloat();
  float modWidth = lBounds.getWidth() / 3.0f;
  auto modArea = lBounds.removeFromTop(modWidth);
  pitch.setBounds(modArea.removeFromLeft(modWidth).toNearestInt());
  mod.setBounds(modArea.toNearestInt());
  // we should have room for one more mod source on the right side here
  auto lfoArea = lBounds.removeFromTop(lBounds.getWidth());
  lfos.setBounds(lfoArea.toNearestInt());
}
