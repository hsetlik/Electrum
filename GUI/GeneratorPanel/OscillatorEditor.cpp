#include "OscillatorEditor.h"

OscillatorEditor::OscillatorEditor(EVT *tree, int idx)
    : state(tree), index(idx),
      sLevel(std::make_unique<ModulationDestSlider>(
          state, IDs::oscillatorLevel.toString() + String(idx))),
      sPos(std::make_unique<ModulationDestSlider>(
          state, IDs::oscillatorPos.toString() + String(idx))),
      label("oscLabel" + String(idx), "Oscillator " + String(idx)),
      graph(tree, idx) {

  addAndMakeVisible(&label);
  addAndMakeVisible(sLevel.get());
  addAndMakeVisible(sPos.get());
  addAndMakeVisible(&graph);
}

void OscillatorEditor::resized() {
  auto lBounds = getLocalBounds().toFloat();
  auto labelRegion = lBounds.removeFromTop(18.0f);
  label.setBounds(labelRegion.reduced(2.0f).toNearestInt());
  auto knobsRegion = lBounds.removeFromTop(lBounds.getHeight() * 0.35f);
  sLevel->setBounds(knobsRegion.removeFromLeft(knobsRegion.getHeight())
                        .reduced(10.0f)
                        .toNearestInt());
  sPos->setBounds(knobsRegion.removeFromLeft(knobsRegion.getHeight())
                      .reduced(10.0f)
                      .toNearestInt());
  auto graphRegion = lBounds.removeFromLeft(lBounds.getWidth() * 0.75f);
  graph.setBounds(graphRegion.toNearestInt());
}

void OscillatorEditor::paint(Graphics &g) {
  g.fillAll(Colour::fromRGB(40, 60, 92));
}
