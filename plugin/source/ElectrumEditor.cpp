#include "Electrum/GUI/ElectrumEditor.h"
#include "Electrum/GUI/Modulation/ModContextComponent.h"
#include "Electrum/PluginProcessor.h"

ElectrumEditor::ElectrumEditor(ElectrumState* s,
                               audio_plugin::ElectrumAudioProcessor* p)
    : ModContextComponent(s),
      state(s),
      processor(p),
      kbdView(p->engine.masterKeyboardState) {
  // set the lookandfeel before adding child components
  setLookAndFeel(&lnf);
  addAndMakeVisible(&kbdView);
}

ElectrumEditor::~ElectrumEditor() {
  setLookAndFeel(nullptr);
}

void ElectrumEditor::resized() {
  auto iBounds = getLocalBounds();
  auto kbdBounds = iBounds.removeFromBottom(85);
  kbdView.setBounds(kbdBounds);
}
//===================================================
