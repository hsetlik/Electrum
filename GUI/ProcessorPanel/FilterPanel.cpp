#include "FilterPanel.h"

FilterPanel::FilterPanel(EVT* tree) : 
state(tree)
{
  sCutoff.reset(new ModulationDestSlider(state, IDs::filterCutoff.toString()));
  sRes.reset(new ModulationDestSlider(state, IDs::filterResonance.toString()));
  sMix.reset(new ModulationDestSlider(state, IDs::filterMix.toString()));
  sTracking.reset(new ModulationDestSlider(state, IDs::filterTracking.toString()));

  addAndMakeVisible(sCutoff.get());
  addAndMakeVisible(sRes.get());
  addAndMakeVisible(sMix.get());
  addAndMakeVisible(sTracking.get());
  //
  bFilterType.addItemList(IDs::filterTypes, 1);
  addAndMakeVisible(&bFilterType);
  bFilterType.setSelectedItemIndex(0);
  typeAttach.reset(new APVTS::ComboBoxAttachment(*state->getAPVTS(), IDs::filterType.toString(), bFilterType));
}


void FilterPanel::resized()
{
  auto fBounds = getLocalBounds().toFloat();
  const float comboBoxHeight = 25.0f;
  auto typeBoxBounds = fBounds.removeFromTop(comboBoxHeight);
  bFilterType.setBounds(typeBoxBounds.toNearestInt());
  auto dX = fBounds.getWidth() / 4.0f;
  const float cushion = 6.0f;
  sCutoff->setBounds(fBounds.removeFromLeft(dX).reduced(cushion).toNearestInt());
  sRes->setBounds(fBounds.removeFromLeft(dX).reduced(cushion).toNearestInt());
  sTracking->setBounds(fBounds.removeFromLeft(dX).reduced(cushion).toNearestInt());
  sMix->setBounds(fBounds.reduced(cushion).toNearestInt());

}
