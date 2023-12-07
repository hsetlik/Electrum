#include "OscillatorEditor.h"
#include <memory>

OscillatorEditor::OscillatorEditor(EVT *tree, int idx)
    : state(tree), index(idx),
      sLevel(std::make_unique<ModulationDestSlider>(
          state, IDs::oscillatorLevel.toString() + String(idx))),
      sPos(std::make_unique<ModulationDestSlider>(
          state, IDs::oscillatorPos.toString() + String(idx))),
      sCoarse(std::make_unique<ModulationDestSlider>(
          state, IDs::oscillatorCoarseTune.toString() + String(idx))),
      sFine(std::make_unique<ModulationDestSlider>(
          state, IDs::oscillatorFineTune.toString() + String(idx))),
      label("oscLabel" + String(idx), "Oscillator " + String(idx)),
      graph(tree, idx)
{

  addAndMakeVisible(&label);
  addAndMakeVisible(sLevel.get());
  addAndMakeVisible(sPos.get());
  addAndMakeVisible(sCoarse.get());
  addAndMakeVisible(sFine.get());
  addAndMakeVisible(&graph);
}

void OscillatorEditor::resized()
{
  auto lBounds = getLocalBounds().toFloat();
  auto labelRegion = lBounds.removeFromTop(18.0f);
  label.setBounds(labelRegion.reduced(2.0f).toNearestInt());
  auto knobsRegion = lBounds.removeFromTop(lBounds.getHeight() * 0.35f);
  auto tuningRegion =
      knobsRegion.removeFromBottom(knobsRegion.getHeight() * 0.5f);
  sCoarse->setBounds(tuningRegion.removeFromLeft(tuningRegion.getHeight())
                         .reduced(1.0f)
                         .toNearestInt());
  sFine->setBounds(tuningRegion.removeFromLeft(tuningRegion.getHeight())
                       .reduced(1.0f)
                       .toNearestInt());
  sLevel->setBounds(knobsRegion.removeFromLeft(knobsRegion.getHeight())
                        .reduced(1.0f)
                        .toNearestInt());
  sPos->setBounds(knobsRegion.removeFromLeft(knobsRegion.getHeight())
                      .reduced(1.0f)
                      .toNearestInt());
  auto graphRegion = lBounds.removeFromLeft(lBounds.getWidth() * 0.75f);
  graph.setBounds(graphRegion.toNearestInt());
}

void OscillatorEditor::paint(Graphics &g)
{
  g.fillAll(Colour::fromRGB(40, 60, 92));
}
