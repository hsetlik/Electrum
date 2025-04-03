
#include "Electrum/GUI/Modulation/ModSourceButton.h"
#include "Electrum/GUI/GUITypedefs.h"
#include "Electrum/GUI/LookAndFeel/Color.h"
#include "Electrum/GUI/LookAndFeel/Fonts.h"
#include "Electrum/Identifiers.h"

frect_t ModSourceButton::getSrcBounds() {
  auto fBounds = getLocalBounds().toFloat();
  static const float idealSrcWidth = 35.0f;
  static const float srcInset = 3.5f;
  const float shortSide = std::min(fBounds.getWidth(), fBounds.getHeight());
  jassert(shortSide >= idealSrcWidth);
  auto r = fBounds.removeFromLeft(idealSrcWidth);
  return r.reduced(srcInset);
}

juce::TextLayout ModSourceButton::getTextLayout(float leftEdge,
                                                bool highlighted) {
  auto safeTextBounds = getLocalBounds().toFloat();
  safeTextBounds.removeFromLeft(leftEdge);
  safeTextBounds = safeTextBounds.reduced(1.2f);
  AttString aStr(btnText);
  auto color = getLookAndFeel().findColour(juce::TextButton::textColourOffId);
  if (highlighted) {
    color = getLookAndFeel().findColour(juce::TextButton::textColourOnId);
  }
  aStr.setColour(color);
  aStr.setFont(FontData::getFontWithHeight(FontE::FuturaLC, 14.0f));
  aStr.setJustification(juce::Justification::centredRight);
  juce::TextLayout tl;
  tl.createLayout(aStr, safeTextBounds.getWidth());
  return tl;
}

//===================================================
ModSourceButton::ModSourceButton(ElectrumState* s,
                                 int srcID,
                                 const String& text)
    : juce::Button("ModSrc" + text), srcComp(s, srcID), btnText(text) {
  addAndMakeVisible(srcComp);
}

void ModSourceButton::paintButton(juce::Graphics& g,
                                  bool btnHighlighted,
                                  bool btnDown) {
  // 1. draw the button background
  auto fBounds = getLocalBounds().toFloat();
  static const float corner = 2.5f;
  g.setColour(getLookAndFeel().findColour(juce::TextButton::buttonColourId));
  g.fillRoundedRectangle(fBounds, corner);
  auto outline = btnDown ? Color::darkBlue : Color::darkBlue.brighter(0.2f);
  g.setColour(outline);
  g.drawRoundedRectangle(fBounds, corner, 1.3f);

  auto sBounds = getSrcBounds();
  auto tl = getTextLayout(sBounds.getRight(), btnHighlighted || btnDown);
  auto lBounds = fBounds;
  lBounds.removeFromLeft(sBounds.getRight());
  tl.draw(g, lBounds);
}
void ModSourceButton::resized() {
  srcComp.setBounds(getSrcBounds().toNearestInt());
}
