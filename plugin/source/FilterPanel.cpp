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
  // 3. set up the button
  addAndMakeVisible(btn);
  const String activeID = ID::filterActive + String(filterIdx);
  bAttach.reset(new apvts::ButtonAttachment(*s, activeID, btn));
}

void FilterComp::resized() {
  static const float upperBar = 27.0f;
  auto fBounds = getLocalBounds().toFloat();
  auto topBounds = fBounds.removeFromTop(upperBar);
  auto bBounds = topBounds.removeFromRight(upperBar);
  bBounds = bBounds.withSizeKeepingCentre(23.0f, 23.0f);
  btn.setBounds(bBounds.toNearestInt());
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
