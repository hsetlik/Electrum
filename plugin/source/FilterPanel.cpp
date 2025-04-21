#include "Electrum/GUI/ProcessPanel/FilterPanel.h"
#include "Electrum/GUI/LookAndFeel/Color.h"
#include "Electrum/GUI/LookAndFeel/Fonts.h"
#include "Electrum/Identifiers.h"

FilterComp::FilterComp(ElectrumState* s, int filterIdx)
    : sCutoff(s, ((int)ModDestE::filt1Cutoff) + (3 * filterIdx)),
      sResonance(s, ((int)ModDestE::filt1Cutoff) + (3 * filterIdx) + 1),
      sGain(s, ((int)ModDestE::filt1Cutoff) + (3 * filterIdx) + 2) {
  // 1. add the sliders
  addAndMakeVisible(sCutoff);
  addAndMakeVisible(sResonance);
  addAndMakeVisible(sGain);
  // 2. set up the name text
  nameStr.aString.setText("Filter " + String(filterIdx + 1));
  nameStr.aString.setFont(FontData::getFontWithHeight(FontE::RobotoMI, 16.0f));
  nameStr.aString.setColour(UIColor::defaultText);
  nameStr.aString.setJustification(juce::Justification::centred);
  // 3. set up the button
  addAndMakeVisible(btn);
  const String activeID = ID::filterActive + String(filterIdx);
  bAttach.reset(new apvts::ButtonAttachment(*s, activeID, btn));
  // 4. set up the type selector box
  addAndMakeVisible(typeBox);
  typeBox.addItemList(getFilterTypeNames(), 1);
  typeBox.setSelectedItemIndex(0);
  const String typeID = ID::filterType.toString() + String(filterIdx);
  cAttach.reset(new apvts::ComboBoxAttachment(*s, typeID, typeBox));
}

void FilterComp::resized() {
  static const float upperBar = 35.0f;
  auto fBounds = getLocalBounds().toFloat();
  auto topBounds = fBounds.removeFromTop(upperBar);
  auto bBounds = topBounds.removeFromRight(upperBar);
  bBounds = bBounds.withSizeKeepingCentre(23.0f, 23.0f);
  btn.setBounds(bBounds.toNearestInt());
  float comboWidth = std::max(topBounds.getWidth() / 2.0f, 65.0f);
  auto nBounds = topBounds.removeFromRight(comboWidth).reduced(3.0f);
  typeBox.setBounds(nBounds.toNearestInt());

  nameStr.bounds = topBounds;
  static const float sliderInset = 3.0f;
  auto topRow = fBounds.removeFromTop(fBounds.getHeight() / 2.0f);
  auto cutoffBounds =
      topRow.removeFromLeft(topRow.getWidth() / 2.0f).reduced(sliderInset);
  auto resBounds = topRow.reduced(sliderInset);
  auto gainBounds =
      fBounds.removeFromLeft(fBounds.getWidth() / 2.0f).reduced(sliderInset);
  sCutoff.setBounds(cutoffBounds.toNearestInt());
  sResonance.setBounds(resBounds.toNearestInt());
  sGain.setBounds(gainBounds.toNearestInt());
}

void FilterComp::paint(juce::Graphics& g) {
  auto fBounds = getLocalBounds().toFloat();
  g.setColour(UIColor::windowBkgnd);
  g.fillRect(fBounds);
  nameStr.draw(g);
}

//===================================================

FilterTabs::FilterTabs(ElectrumState* s)
    : juce::TabbedComponent(TabBar::Orientation::TabsAtLeft) {
  for (int i = 0; i < NUM_FILTERS; ++i) {
    String name = "VCF " + String(i + 1);
    addTab(name, UIColor::widgetBkgnd, new FilterComp(s, i), true);
  }
}
