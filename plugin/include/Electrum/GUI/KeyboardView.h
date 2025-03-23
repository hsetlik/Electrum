#pragma once
#include "Electrum/Common.h"
#include "juce_audio_utils/juce_audio_utils.h"
#include "juce_gui_basics/juce_gui_basics.h"

class KeyboardView : public Component {
private:
  juce::MidiKeyboardComponent core;

public:
  KeyboardView(juce::MidiKeyboardState& state);
  void resized() override;
};

