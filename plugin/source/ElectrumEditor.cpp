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
      kbdView(p->engine.masterKeyboardState) {
  // set the lookandfeel before adding child components
  setLookAndFeel(&lnf);
  addAndMakeVisible(&kbdView);
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
  const int oscPanelHeight = std::min(iBounds.getHeight(), 500);
  const int leftPanelWidth = std::min(iBounds.getWidth() / 5, 400);
  auto leftPanel = iBounds.removeFromLeft(leftPanelWidth);
  // whatever we decide to put on the left eventually
  juce::ignoreUnused(leftPanel);
  auto fOscArea = iBounds.removeFromTop(oscPanelHeight).toFloat();
  const float fOscWidth = fOscArea.getWidth() / 3.0f;
  for (auto* o : oscs) {
    auto fBounds = fOscArea.removeFromLeft(fOscWidth);
    o->setBounds(fBounds.toNearestInt());
  }
  kbdView.setBounds(kbdBounds);
}
//===================================================
