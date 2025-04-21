#include "Electrum/GUI/ElectrumEditor.h"
#include "Electrum/GUI/Modulation/ModContextComponent.h"
#include "Electrum/GUI/Wavetable/OscillatorPanel.h"
#include "Electrum/Identifiers.h"
#include "Electrum/PluginProcessor.h"
#include "juce_core/juce_core.h"

ElectrumMainView::ElectrumMainView(ElectrumState* s,
                                   audio_plugin::ElectrumAudioProcessor* p)
    : ModContextComponent(s),
      state(s),
      processor(p),
      kbdView(p->engine.masterKeyboardState),
      filterTabs(s),
      macroPanel(s),
      browser(s),
      envPanel(s) {
  addAndMakeVisible(&kbdView);
  addAndMakeVisible(&filterTabs);
  addAndMakeVisible(&macroPanel);
  addAndMakeVisible(&envPanel);
  addAndMakeVisible(&browser);
  // add the oscs
  for (int i = 0; i < NUM_OSCILLATORS; ++i) {
    auto* osc = oscs.add(new OscillatorPanel(state, i));
    addAndMakeVisible(osc);
  }
}

void ElectrumMainView::resized() {
  auto iBounds = getLocalBounds();
  auto kbdBounds = iBounds.removeFromBottom(85);
  kbdView.setBounds(kbdBounds);
  static const int oscPanelHeight = std::min(iBounds.getHeight(), 500);
  static const int leftPanelWidth = std::min(iBounds.getWidth() / 5, 400);
  auto leftPanel = iBounds.removeFromLeft(leftPanelWidth);
  const int macroPanelHeight = std::max(leftPanel.getHeight() / 7, 110);
  macroPanel.setBounds(leftPanel.removeFromTop(macroPanelHeight));
  browser.setBounds(leftPanel);

  auto fOscArea = iBounds.removeFromTop(oscPanelHeight).toFloat();
  const float fOscWidth = fOscArea.getWidth() / 3.0f;
  for (auto* o : oscs) {
    auto fBounds = fOscArea.removeFromLeft(fOscWidth);
    o->setBounds(fBounds.toNearestInt());
  }
  static const int envHeight = 400;
  auto midPanel = iBounds.removeFromTop(envHeight);
  static const int envWidth = std::min(iBounds.getWidth() / 3, 450);
  filterTabs.setBounds(midPanel.removeFromLeft(envWidth));
  envPanel.setBounds(midPanel.removeFromLeft(envWidth));
  // TODO: put the LFOs in the rest of this row
}
//===================================================

ElectrumEditor::ElectrumEditor(ElectrumState* s,
                               audio_plugin::ElectrumAudioProcessor* p)
    : mainView(s, p), waveView(nullptr) {
  setLookAndFeel(&lnf);
  addAndMakeVisible(mainView);
}

ElectrumEditor::~ElectrumEditor() {
  setLookAndFeel(nullptr);
}

void ElectrumEditor::_openWaveEditor(ElectrumState* s, Wavetable* wt, int idx) {
  waveView.reset(new WaveEditor(s, wt, idx));
  mainView.setEnabled(false);
  addAndMakeVisible(waveView.get());
  waveViewOpen = true;
  resized();
}

void ElectrumEditor::_exitModalView() {
  waveView.reset(nullptr);
  waveViewOpen = false;
  mainView.setEnabled(true);
  resized();
}

void ElectrumEditor::resized() {
  auto iBounds = getLocalBounds();
  mainView.setBounds(iBounds);
  static const int maxModalW = 1000;
  static const int maxModalH = 800;
  if (waveViewOpen) {
    auto wBounds = iBounds.withSizeKeepingCentre(maxModalW, maxModalH);
    waveView->setBounds(wBounds);
    waveView->toFront(true);
  }
}
