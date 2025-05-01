#include "Electrum/GUI/ModulatorPanel/LevelSourceComponent.h"
#include "Electrum/GUI/LookAndFeel/Color.h"
#include "Electrum/GUI/LookAndFeel/Fonts.h"
#include "Electrum/Identifiers.h"
#include "Electrum/Shared/ElectrumState.h"

LevelSourceComponent::LevelSourceComponent(ElectrumState* s)
    : state(s),
      monoSrc(s, (int)ModSourceE::LevelMono),
      polySrc(s, (int)ModSourceE::LevelPoly) {
  addAndMakeVisible(monoSrc);
  addAndMakeVisible(polySrc);
  state->graph.addListener(this);
}

LevelSourceComponent::~LevelSourceComponent() {
  state->graph.removeListener(this);
}

void LevelSourceComponent::graphingDataUpdated(GraphingData* gd) {
  const float _mono = gd->getMonoLevel();
  const float _poly = gd->getPolyLevel();
  if (!fequal(_poly, lastPolyLevel) || !fequal(_mono, lastMonoLevel)) {
    lastPolyLevel = std::clamp(_poly, 0.0f, 1.0f);
    lastMonoLevel = std::clamp(_mono, 0.0f, 1.0f);
    triggerAsyncUpdate();
  }
}

void LevelSourceComponent::enablementChanged() {}

void LevelSourceComponent::resized() {
  auto rBounds = getLocalBounds().toFloat();
  auto lBounds = rBounds.removeFromLeft(rBounds.getWidth() / 2.0f);
  monoLabel.bounds = lBounds.removeFromTop(22.0f);
  polyLabel.bounds = rBounds.removeFromTop(22.0f);
  static const float srcWidth = 30.0f;
  lBounds = lBounds.withSizeKeepingCentre(srcWidth, srcWidth);
  rBounds = rBounds.withSizeKeepingCentre(srcWidth, srcWidth);
  monoSrc.setBounds(lBounds.toNearestInt());
  polySrc.setBounds(rBounds.toNearestInt());
}

static float levelDisplayMid = juce::Decibels::decibelsToGain(-12.0f);
static frange_t levelYCurve = rangeWithCenter(0.0f, 1.0f, levelDisplayMid);

void LevelSourceComponent::paint(juce::Graphics& g) {
  monoLabel.aString.setFont(
      FontData::getFontWithHeight(FontE::RobotoMI, 16.0f));
  monoLabel.aString.setJustification(juce::Justification::centred);
  monoLabel.aString.setColour(UIColor::defaultText);
  monoLabel.aString.setText("Mono Level");

  polyLabel.aString.setFont(
      FontData::getFontWithHeight(FontE::RobotoMI, 16.0f));
  polyLabel.aString.setJustification(juce::Justification::centred);
  polyLabel.aString.setColour(UIColor::defaultText);
  polyLabel.aString.setText("Poly Level");
  auto rBounds = getLocalBounds().toFloat();
  g.setColour(UIColor::windowBkgnd);
  g.fillRect(rBounds);
  auto lBounds = rBounds.removeFromLeft(rBounds.getWidth() / 2.0f);
  g.setColour(Color::marginGray);
  g.drawRect(rBounds, 2.0f);
  g.drawRect(lBounds, 2.0f);
  g.setColour(Color::periwinkle.withAlpha(0.8f));
  auto nMono = levelYCurve.convertTo0to1(lastMonoLevel);
  auto nPoly = levelYCurve.convertTo0to1(lastPolyLevel);
  auto monoRect = lBounds.removeFromBottom(lBounds.getHeight() * nMono);
  g.fillRect(monoRect);
  auto polyRect = rBounds.removeFromBottom(rBounds.getHeight() * nPoly);
  g.fillRect(polyRect);
  monoLabel.draw(g);
  polyLabel.draw(g);
}
//===================================================
