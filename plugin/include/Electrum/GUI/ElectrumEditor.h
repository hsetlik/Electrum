#pragma once

#include "Electrum/GUI/KeyboardView.h"
#include "Electrum/GUI/LookAndFeel/ElectrumLnF.h"
#include "Electrum/GUI/ModulatorPanel/EnvelopeComponent.h"
#include "Electrum/GUI/ModulatorPanel/MacroModSources.h"
#include "Electrum/GUI/Wavetable/OscillatorPanel.h"
#include "Electrum/PluginProcessor.h"
#include "Modulation/ModContextComponent.h"
#include "juce_audio_basics/juce_audio_basics.h"
#include "juce_gui_basics/juce_gui_basics.h"

class ElectrumEditor : public ModContextComponent,
                       public juce::DragAndDropContainer {
private:
  ElectrumState* const state;
  audio_plugin::ElectrumAudioProcessor* const processor;
  // lnf
  ElectrumLnF lnf;
  // keyboard stuff
  KeyboardView kbdView;
  // Oscillator views
  juce::OwnedArray<OscillatorPanel> oscs;
  // Left panel modulators
  MacroModSources macroPanel;
  // Envelope panel
  EnvGroupComponent envPanel;

public:
  ElectrumEditor(ElectrumState* s, audio_plugin::ElectrumAudioProcessor* p);
  ~ElectrumEditor() override;
  void resized() override;
};
