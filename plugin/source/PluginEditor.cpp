#include "Electrum/PluginEditor.h"
#include "Electrum/PluginProcessor.h"

namespace audio_plugin {
ElectrumProcessorEditor::ElectrumProcessorEditor(ElectrumAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p), ed(&p.tree, &p) {
  juce::ignoreUnused(processorRef);

  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.
  addAndMakeVisible(ed);
  setSize(1800, 1200);
}

ElectrumProcessorEditor::~ElectrumProcessorEditor() {}

void ElectrumProcessorEditor::paint(juce::Graphics& g) {
  // (Our component is opaque, so we must completely fill the background with a
  // solid colour)
}

void ElectrumProcessorEditor::resized() {
  // This is generally where you'll want to lay out the positions of any
  // subcomponents in your editor..
  ed.setBounds(getLocalBounds());
}
}  // namespace audio_plugin
