#include "Electrum/GUI/WaveEditor/WaveEditor.h"
#include "Electrum/GUI/LookAndFeel/Color.h"
#include "Electrum/GUI/WaveEditor/EditValueTree.h"
#include "Electrum/Identifiers.h"

//===================================================

WaveEditor::WaveEditor(ElectrumState* s, Wavetable* wt, int idx)
    : state(s), wavetable(wt), oscID(idx) {
  // 1. figure out which file we need to load
  String pathID = ID::oscWavePath.toString() + String(oscID);
  String path = state->state.getProperty(pathID);
  // 2. parse as a valueTree
  waveTree = WaveEdit::getWavetableTree(path);
  jassert(waveTree.isValid());
}

WaveEditor::~WaveEditor() {
  auto parent = getParentComponent();
  if (parent != nullptr) {
    parent->removeChildComponent(this);
  }
}

void WaveEditor::resized() {}

void WaveEditor::paint(juce::Graphics& g) {
  auto fBounds = getLocalBounds().toFloat();
  g.setColour(UIColor::widgetBkgnd);
  g.fillRect(fBounds);
}
