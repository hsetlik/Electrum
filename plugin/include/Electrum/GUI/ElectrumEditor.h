#pragma once

#include "Electrum/GUI/KeyboardView.h"
#include "Electrum/GUI/LFOEditor/LFOEdit.h"
#include "Electrum/GUI/LookAndFeel/ElectrumLnF.h"
#include "Electrum/GUI/ModulatorPanel/EnvelopeComponent.h"
#include "Electrum/GUI/ModulatorPanel/LFOComponent.h"
#include "Electrum/GUI/ModulatorPanel/LevelSourceComponent.h"
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
  // audio_plugin::ElectrumAudioProcessor* const processor;

  // keyboard stuff
  KeyboardView kbdView;
  // Oscillator views
  juce::OwnedArray<OscillatorPanel> oscs;
  // Filter panels
  FilterTabs filterTabs;
  // Left panel modulators
  MacroModSources macroPanel;
  // patch browser panel
  PatchBrowser browser;
  // Envelope panel
  EnvGroupComponent envPanel;
  // LFO panel
  LFOTabs lfoPanel;
  // Level mod sources
  LevelSourceComponent levelComp;

public:
  ElectrumMainView(ElectrumState* s, audio_plugin::ElectrumAudioProcessor* p);
  ~ElectrumMainView() override;
  void resized() override;
};

//=========================================================================
class ElectrumEditor : public ModalParent {
private:
  // lnf
  ElectrumLnF lnf;

  ElectrumMainView mainView;
  std::unique_ptr<WaveEditor> waveView;
  std::unique_ptr<LFOEditor> lfoView;
  Component* currentModalComp = nullptr;
  bool modalIsOpen = false;
  void _openWaveEditor(ElectrumState* s, Wavetable* wt, int idx) override;
  void _openLFOEditor(ElectrumState* s, int idx) override;
  void _exitModalView() override;

public:
  ElectrumEditor(ElectrumState* s, audio_plugin::ElectrumAudioProcessor* p);
  ~ElectrumEditor() override;
  void resized() override;
};
