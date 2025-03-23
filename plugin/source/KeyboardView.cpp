#include "Electrum/GUI/KeyboardView.h"
#include "juce_audio_basics/juce_audio_basics.h"
#include "juce_audio_utils/juce_audio_utils.h"

KeyboardView::KeyboardView(juce::MidiKeyboardState& state)
    : core(state, juce::KeyboardComponentBase::horizontalKeyboard) {
  addAndMakeVisible(core);
}

void KeyboardView::resized() {
  core.setBounds(getLocalBounds());
}

//===================================================
