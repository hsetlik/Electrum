#include "HeaderPanel.h"
HeaderLabel::HeaderLabel()
{
  setText("Electrum", juce::dontSendNotification);
  Font f(Fonts::getTypeface(Fonts::FuturaBoldOblique));
  setFont(f.withHeight(HEADER_FONT_PX));
  setBorderSize(BorderSize<int>(5));
  setJustificationType(Justification::centred);
}

HeaderPanel::HeaderPanel(EVT *tree) : state(tree) { addAndMakeVisible(&label); }

void HeaderPanel::resized()
{
  auto fBounds = getLocalBounds().toFloat();
  auto labelBounds =
      fBounds.removeFromTop(HEADER_FONT_PX + (float)label.getBorderSize().getTopAndBottom());
  label.setBounds(labelBounds.toNearestInt());
}

void HeaderPanel::paint(Graphics &g)
{
  auto bg = getLookAndFeel().findColour(Label::ColourIds::backgroundColourId);
  g.fillAll(bg);
}
