#pragma once

#include "Electrum/GUI/KeyboardView.h"
#include "Electrum/GUI/LookAndFeel/ElectrumLnF.h"
#include "Electrum/GUI/ModulatorPanel/EnvelopeComponent.h"
#include "Electrum/GUI/ModulatorPanel/MacroModSources.h"
#include "Electrum/GUI/PatchBrowser.h"
#include "Electrum/GUI/ProcessPanel/FilterPanel.h"
#include "Electrum/GUI/Util/ModalParent.h"
#include "Electrum/GUI/Wavetable/OscillatorPanel.h"
#include "WaveEditor/WaveEditor.h"
#include "Electrum/PluginProcessor.h"
#include "Modulation/ModContextComponent.h"
#include "juce_gui_basics/juce_gui_basics.h"

class ElectrumMainView : public ModContextComponent,
                         public juce::DragAndDropContainer {
private:
  ElectrumState* const state;
  audio_plugin::ElectrumAudioProcessor* const processor;

  // keyboard stuff
  KeyboardView kbdView;
  // Oscillator views
  juce::OwnedArray<OscillatorPanel> oscs;
  // Filter panels
  juce::OwnedArray<FilterComp> filters;
  // Left panel modulators
  MacroModSources macroPanel;
  // patch browser panel
  PatchBrowser browser;
  // Envelope panel
  EnvGroupComponent envPanel;

public:
  ElectrumMainView(ElectrumState* s, audio_plugin::ElectrumAudioProcessor* p);
  void resized() override;
};

//=========================================================================
class ElectrumEditor : public ModalParent {
private:
  // lnf
  ElectrumLnF lnf;

  ElectrumMainView mainView;
  std::unique_ptr<WaveEditor> waveView;
  bool waveViewOpen = false;
  void _openWaveEditor(ElectrumState* s, Wavetable* wt, int idx) override;
  void _exitModalView() override;

public:
  ElectrumEditor(ElectrumState* s, audio_plugin::ElectrumAudioProcessor* p);
  ~ElectrumEditor() override;
  void resized() override;
};
