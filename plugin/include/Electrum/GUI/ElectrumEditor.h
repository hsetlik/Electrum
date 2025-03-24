#pragma once

#include "Electrum/GUI/KeyboardView.h"
#include "Electrum/GUI/LookAndFeel/ElectrumLnF.h"
#include "Electrum/PluginProcessor.h"
#include "Modulation/ModContextComponent.h"
#include "juce_audio_basics/juce_audio_basics.h"

class ElectrumEditor : public ModContextComponent {
private:
  ElectrumState* const state;
  audio_plugin::ElectrumAudioProcessor* const processor;
  // lnf
  ElectrumLnF lnf;
  // keyboard stuff
  KeyboardView kbdView;

public:
  ElectrumEditor(ElectrumState* s, audio_plugin::ElectrumAudioProcessor* p);
  ~ElectrumEditor() override;
  void resized() override;
};
