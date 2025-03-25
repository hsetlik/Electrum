#include "Electrum/PluginEditor.h"
#include "Electrum/PluginProcessor.h"
#include "juce_core/juce_core.h"

namespace audio_plugin {
ElectrumProcessorEditor::ElectrumProcessorEditor(ElectrumAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p), ed(&p.tree, &p) {
  juce::ignoreUnused(processorRef);

  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.
  addAndMakeVisible(ed);
  // we'll figure out the editor's size based
  // on the size of the area that the host/OS is
  // allowing us to use
  auto* constrainer = getConstrainer();
  const int edWidth = std::min(constrainer->getMaximumWidth(), 1600);
  const int edHeight = std::min(constrainer->getMaximumHeight(), 1200);
  setSize(edWidth, edHeight);
}

ElectrumProcessorEditor::~ElectrumProcessorEditor() {}

void ElectrumProcessorEditor::paint(juce::Graphics& g) {
  juce::ignoreUnused(g);
  // (Our component is opaque, so we must completely fill the background with a
  // solid colour)
}

void ElectrumProcessorEditor::resized() {
  // This is generally where you'll want to lay out the positions of any
  // subcomponents in your editor..
  ed.setBounds(getLocalBounds());
}
}  // namespace audio_plugin
