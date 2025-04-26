#include "Electrum/GUI/Util/BeatSyncToggle.h"
#include "Electrum/GUI/GUITypedefs.h"
#include "Electrum/GUI/LookAndFeel/Color.h"
#include "Electrum/GUI/LookAndFeel/Fonts.h"

void BeatSyncToggle::paintButton(juce::Graphics& g, bool, bool) {
  static const float corner = 3.0f;
  static const float border = 2.0f;
  static const color_t background = UIColor::widgetBkgnd;
  auto fBounds = getLocalBounds().toFloat();
  const float maxTextHeight = (fBounds.getHeight() - (2.0f * border)) / 2.0f;
  color_t fill;
  String btnText;
  if (getToggleState()) {
    fill = Color::assignmentPink;
    btnText = "BPM";
  } else {
    fill = Color::periwinkle;
    btnText = "Hz";
  }
  // 1. draw the border and outline
  g.setColour(fill);
  g.fillRoundedRectangle(fBounds, corner);
  fBounds = fBounds.reduced(border);
  g.setColour(background);
  g.fillRoundedRectangle(fBounds, corner);
  // 2. draw the string
  AttString aStr(btnText);
  float fontHeight = std::min(maxTextHeight, 18.0f);
  aStr.setFont(FontData::getFontWithHeight(FontE::RobotoMI, fontHeight));
  aStr.setJustification(juce::Justification::centred);
  aStr.setColour(fill);
  aStr.draw(g, fBounds);
}
//===================================================
