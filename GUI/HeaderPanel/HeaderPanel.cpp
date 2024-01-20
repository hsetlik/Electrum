#include "HeaderPanel.h"
HeaderPanel::HeaderPanel(EVT *tree)
    : state(tree), nameFont(Fonts::getTypeface(Fonts::AcierDisplayNoir)), patchSelector(tree)
{
  addAndMakeVisible(&patchSelector);
}

void HeaderPanel::resized()
{
  auto fBounds = getLocalBounds().toFloat();
  fBounds.removeFromTop(fBounds.getHeight() * 0.26f).toNearestInt();
  const float headerHeight = 16.0f;
  auto sBounds = fBounds.removeFromTop(headerHeight);
  patchSelector.setBounds(sBounds.toNearestInt());
}

void HeaderPanel::paint(Graphics &g)
{
  auto bg = getLookAndFeel().findColour(Label::ColourIds::backgroundColourId);
  g.fillAll(bg);
  auto fBounds = getLocalBounds().toFloat();
  auto textBounds = fBounds.removeFromTop(fBounds.getHeight() * 0.26f).toNearestInt();
  auto textColor = getLookAndFeel().findColour(Label::ColourIds::textColourId);

  g.setColour(textColor);
  g.setFont(nameFont.withHeight((float)(textBounds.getHeight() - 5)));
  g.drawFittedText("Electrum", textBounds, Justification::centredLeft, 1);
}
