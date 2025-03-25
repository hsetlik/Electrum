#include "Electrum/GUI/ModulatorPanel/MacroModSources.h"
#include "Electrum/GUI/LookAndFeel/Color.h"
#include "Electrum/GUI/LookAndFeel/Fonts.h"
#include "Electrum/Shared/ElectrumState.h"
#include "juce_graphics/juce_graphics.h"

constexpr float _labelHeight = 18.0f;

static juce::AttributedString _getModWheelAString() {
  juce::AttributedString aStr("Mod wheel");
  auto font = FontData::getFontWithHeight(FontE::HelveticaMed, 15.0f);
  aStr.setFont(font);
  aStr.setJustification(juce::Justification::centred);
  aStr.setColour(Color::gainsboro);
  return aStr;
}

static juce::AttributedString _getVelocityAString() {
  juce::AttributedString aStr("Velocity");
  auto font = FontData::getFontWithHeight(FontE::HelveticaMed, 15.0f);
  aStr.setFont(font);
  aStr.setJustification(juce::Justification::verticallyCentred);
  aStr.setColour(Color::gainsboro);
  return aStr;
}

//===================================================
constexpr float _maxSrcWidth = 120.0f;
MacroModSources::MacroModSources(ElectrumState* s)
    : modWheelSrc(s, (int)ModSourceE::ModWheel),
      velocitySrc(s, (int)ModSourceE::Velocity) {
  modWheelLabel.createLayout(_getModWheelAString(), _maxSrcWidth);
  velocityLabel.createLayout(_getVelocityAString(), _maxSrcWidth);
  addAndMakeVisible(modWheelSrc);
  addAndMakeVisible(velocitySrc);
}

void MacroModSources::resized() {
  auto fBounds = getLocalBounds().toFloat();
  // NOTE: this line needs to change if we add more components
  const float srcWidth = std::min(fBounds.getWidth() / (float)2, _maxSrcWidth);
  // 1. set up the labels' bounds
  modWheelLabel.createLayout(_getModWheelAString(), srcWidth, _labelHeight);
  velocityLabel.createLayout(_getVelocityAString(), srcWidth, _labelHeight);
  auto modBounds = fBounds.removeFromLeft(srcWidth);
  modBounds.removeFromTop(_labelHeight);

  modWheelSrc.placeCenteredAtMaxSize(modBounds.toNearestInt());
  auto velBounds = fBounds.removeFromLeft(srcWidth);
  velBounds.removeFromTop(_labelHeight);
  velocitySrc.placeCenteredAtMaxSize(velBounds.toNearestInt());
}

void MacroModSources::paint(juce::Graphics& g) {
  auto fBounds = getLocalBounds().toFloat();
  // NOTE: this line needs to change if we add more components
  const float srcWidth = std::min(fBounds.getWidth() / (float)2, _maxSrcWidth);

  auto modLabelRect =
      fBounds.removeFromLeft(srcWidth).removeFromTop(_labelHeight);
  modWheelLabel.draw(g, modLabelRect);
  auto velLabelRect =
      fBounds.removeFromLeft(srcWidth).removeFromTop(_labelHeight);
  velocityLabel.draw(g, velLabelRect);
}
