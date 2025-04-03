#include "Electrum/GUI/ElectrumEditor.h"
#include "Electrum/GUI/Modulation/ModContextComponent.h"
#include "Electrum/GUI/Wavetable/OscillatorPanel.h"
#include "Electrum/Identifiers.h"
#include "Electrum/PluginProcessor.h"
#include "juce_core/juce_core.h"

ElectrumEditor::ElectrumEditor(ElectrumState* s,
                               audio_plugin::ElectrumAudioProcessor* p)
    : ModContextComponent(s),
      state(s),
      processor(p),
      kbdView(p->engine.masterKeyboardState),
      macroPanel(s),
      envPanel(s) {
  // set the lookandfeel before adding child components
  setLookAndFeel(&lnf);
  addAndMakeVisible(&kbdView);
  addAndMakeVisible(&macroPanel);
  addAndMakeVisible(&envPanel);
  // add the oscs
  for (int i = 0; i < NUM_OSCILLATORS; ++i) {
    auto* osc = oscs.add(new OscillatorPanel(state, i));
    addAndMakeVisible(osc);
  }
}

ElectrumEditor::~ElectrumEditor() {
  setLookAndFeel(nullptr);
}

void ElectrumEditor::resized() {
  auto iBounds = getLocalBounds();
  auto kbdBounds = iBounds.removeFromBottom(85);
  kbdView.setBounds(kbdBounds);
  static const int oscPanelHeight = std::min(iBounds.getHeight(), 500);
  static const int leftPanelWidth = std::min(iBounds.getWidth() / 5, 400);
  auto leftPanel = iBounds.removeFromLeft(leftPanelWidth);
  const int macroPanelHeight = std::max(leftPanel.getHeight() / 7, 110);
  macroPanel.setBounds(leftPanel.removeFromTop(macroPanelHeight));

  auto fOscArea = iBounds.removeFromTop(oscPanelHeight).toFloat();
  const float fOscWidth = fOscArea.getWidth() / 3.0f;
  for (auto* o : oscs) {
    auto fBounds = fOscArea.removeFromLeft(fOscWidth);
    o->setBounds(fBounds.toNearestInt());
  }
  static const int envHeight = std::min(iBounds.getHeight() / 3, 250);
  auto midPanel = iBounds.removeFromTop(envHeight);
  static const int envWidth = std::min(iBounds.getWidth() / 3, 450);
  envPanel.setBounds(midPanel.removeFromLeft(envWidth));
}
//===================================================
