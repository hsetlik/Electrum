#include "SaturationPanel.h"

SaturationPanel::SaturationPanel(EVT *tree)
    : state(tree), sCoeff(tree, IDs::saturationCoeff.toString(), "Strength"),
      sDrive(tree, IDs::saturationDrive.toString(), "Drive"),
      sMix(tree, IDs::saturationMix.toString(), "Wet/Dry")
{
  addAndMakeVisible(sCoeff);
  addAndMakeVisible(sDrive);
  addAndMakeVisible(sMix);

  bSatType.addItemList(IDs::satTypes, 1);
  addAndMakeVisible(&bSatType);
  bSatType.setSelectedItemIndex(0);
  typeAttach.reset(
      new APVTS::ComboBoxAttachment(*state->getAPVTS(), IDs::saturationType.toString(), bSatType));
}

SaturationPanel::~SaturationPanel() { setLookAndFeel(nullptr); }

void SaturationPanel::resized()
{
  auto fBounds = getLocalBounds().toFloat();
  auto knobHeight = fBounds.getHeight() / 3.0f;
  auto knobBounds = fBounds.removeFromRight(knobHeight);
  sCoeff.setBounds(knobBounds.removeFromTop(knobHeight).toNearestInt());
  sDrive.setBounds(knobBounds.removeFromTop(knobHeight).toNearestInt());
  sMix.setBounds(knobBounds.toNearestInt());
  auto boxBounds = fBounds.removeFromTop(fBounds.getHeight() / 7.0f).reduced(8.0f).toNearestInt();
  bSatType.setBounds(boxBounds);
}

void SaturationPanel::paint(Graphics &g) {}
