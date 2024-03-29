#include "FilterPanel.h"

FilterPanel::FilterPanel(EVT *tree)
    : state(tree), sCutoff(tree, IDs::filterCutoff.toString(), "Cutoff"),
      sRes(tree, IDs::filterResonance.toString(), "Resonance"),
      sMix(tree, IDs::filterMix.toString(), "Wet/Dry Mix"),
      sTracking(tree, IDs::filterTracking.toString(), "Tracking")
{

  addAndMakeVisible(sCutoff);
  addAndMakeVisible(sRes);
  addAndMakeVisible(sMix);
  addAndMakeVisible(sTracking);

  sCutoff.setTextColor(Color::offWhite);
  sRes.setTextColor(Color::offWhite);
  sTracking.setTextColor(Color::offWhite);
  sMix.setTextColor(Color::offWhite);
  //
  bFilterType.addItemList(IDs::filterTypes, 1);
  addAndMakeVisible(&bFilterType);
  bFilterType.setSelectedItemIndex(0);
  typeAttach.reset(
      new APVTS::ComboBoxAttachment(*state->getAPVTS(), IDs::filterType.toString(), bFilterType));
}

FilterPanel::~FilterPanel() { setLookAndFeel(nullptr); }

void FilterPanel::resized()
{
  auto fBounds = getLocalBounds().toFloat();
  const float comboBoxHeight = 25.0f;
  auto typeBoxBounds = fBounds.removeFromTop(comboBoxHeight);
  bFilterType.setBounds(typeBoxBounds.toNearestInt());
  auto dX = fBounds.getWidth() / 4.0f;
  const float cushion = 6.0f;
  sCutoff.setBounds(fBounds.removeFromLeft(dX).reduced(cushion).toNearestInt());
  sRes.setBounds(fBounds.removeFromLeft(dX).reduced(cushion).toNearestInt());
  sTracking.setBounds(fBounds.removeFromLeft(dX).reduced(cushion).toNearestInt());
  sMix.setBounds(fBounds.reduced(cushion).toNearestInt());
}
