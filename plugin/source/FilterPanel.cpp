#include "Electrum/GUI/ProcessPanel/FilterPanel.h"
#include "Electrum/GUI/LookAndFeel/Color.h"
#include "Electrum/GUI/LookAndFeel/Fonts.h"
#include "Electrum/Identifiers.h"

void FilterRoutingToggle::paintButton(juce::Graphics& g, bool, bool) {
  auto fBounds = getLocalBounds().toFloat();
  auto width = fBounds.getWidth();
  auto height = std::min(width / 2.0f, fBounds.getHeight());
  fBounds = fBounds.withSizeKeepingCentre(width, height);
  // 1. draw the background
  const float corner = fBounds.getHeight() / 2.0f;
  auto fill = getToggleState() ? Color::literalOrangeBright : Color::darkBlue;
  g.setColour(fill);
  g.fillRoundedRectangle(fBounds, corner);
  g.setColour(Color::marginGray);
  frect_t tBounds;
  if (getToggleState()) {
    tBounds = fBounds.removeFromRight(height);
  } else {
    tBounds = fBounds.removeFromLeft(height);
  }
  g.fillEllipse(tBounds.reduced(2.5f));
}

FilterRoutingComp::FilterRoutingComp(ElectrumState* s, int index) {
  const String iStr(index);
  for (int o = 0; o < NUM_OSCILLATORS; ++o) {
    String oStr(o + 1);
    const String paramID = "filterOsc" + oStr + "On" + iStr;
    auto& btn = *buttons.add(new FilterRoutingToggle());
    addAndMakeVisible(btn);
    attachments.add(new apvts::ButtonAttachment(*s, paramID, btn));
    BoundedAttString label;
    label.aString.setText("Osc. " + oStr);
    label.aString.setFont(FontData::getFontWithHeight(FontE::RobotoMI, 14.0f));
    label.aString.setColour(UIColor::defaultText);
    label.aString.setJustification(juce::Justification::centred);
    labels.push_back(label);
  }
}

void FilterRoutingComp::resized() {
  auto fBounds = getLocalBounds().toFloat().reduced(3.0f);
  const float dY = fBounds.getHeight() / (float)(2 * NUM_OSCILLATORS);
  for (int i = 0; i < NUM_OSCILLATORS; ++i) {
    labels[(size_t)i].bounds = fBounds.removeFromTop(dY);
    buttons[i]->setBounds(
        fBounds.removeFromTop(dY).reduced(2.0f).toNearestInt());
  }
}

void FilterRoutingComp::paint(juce::Graphics& g) {
  auto fBounds = getLocalBounds().toFloat();
  g.setColour(UIColor::windowBkgnd);
  g.fillRect(fBounds);
  g.setColour(Color::marginGray);
  g.drawRect(fBounds, 2.0f);
  for (auto& l : labels) {
    l.draw(g);
  }
}

//==================================
FilterComp::FilterComp(ElectrumState* s, int filterIdx)
    : router(s, filterIdx),
      sCutoff(s, ((int)ModDestE::filt1Cutoff) + (3 * filterIdx)),
      sResonance(s, ((int)ModDestE::filt1Cutoff) + (3 * filterIdx) + 1),
      sGain(s, ((int)ModDestE::filt1Cutoff) + (3 * filterIdx) + 2) {
  // 1. add the sliders
  addAndMakeVisible(router);
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

  auto rBounds = fBounds.removeFromLeft(fBounds.getHeight() / 6.0f);
  router.setBounds(rBounds.toNearestInt());

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
