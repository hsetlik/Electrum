#pragma once
#include "Electrum/GUI/GUITypedefs.h"
#include "Electrum/Identifiers.h"
#include "ModSourceComponent.h"

// a component that works like a button
// with an integrated ModSourceComponent

class ModSourceButton : public juce::Button {
private:
  ModSourceComponent srcComp;
  frect_t getSrcBounds();
  juce::TextLayout getTextLayout(float leftTrim, bool highlighted);
  String btnText;

public:
  ModSourceButton(ElectrumState* s, int srcID, const String& text = "null");
  void setBtnText(const String& txt) { btnText = txt; }
  void paintButton(juce::Graphics& g,
                   bool btnHighlighted,
                   bool btnDown) override;
  void resized() override;
};
