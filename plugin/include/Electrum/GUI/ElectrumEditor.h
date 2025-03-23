#pragma once

#include "Electrum/GUI/KeyboardView.h"
#include "Electrum/PluginProcessor.h"
#include "Modulation/ModContextComponent.h"
#include "juce_audio_basics/juce_audio_basics.h"

class ElectrumEditor : public ModContextComponent {
private:
  ElectrumState* const state;
  audio_plugin::ElectrumAudioProcessor* const processor;
  // keyboard stuff
  KeyboardView kbdView;

public:
  ElectrumEditor(ElectrumState* s, audio_plugin::ElectrumAudioProcessor* p);
  void resized() override;
};
