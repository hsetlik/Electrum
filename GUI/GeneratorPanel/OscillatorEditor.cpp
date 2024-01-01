#include "FontBinaries.h"
#include "OscillatorEditor.h"
#include <memory>

OscillatorEditor::OscillatorEditor(EVT *tree, int idx)
    : state(tree), index(idx),
      sLevel(state, IDs::oscillatorLevel.toString() + String(idx), "Level"),
      sPos(state, IDs::oscillatorPos.toString() + String(idx), "Position"),
      sPan(state, IDs::oscillatorPan.toString() + String(idx), "Pan"),
      sCoarse(state, IDs::oscillatorCoarseTune.toString() + String(idx), "Coarse"),
      sFine(state, IDs::oscillatorFineTune.toString() + String(idx), "Fine"),
      label("oscLabel" + String(idx), "Oscillator " + String(idx)), graph(tree, idx)
{
  //  addAndMakeVisible(&label);
  addAndMakeVisible(sLevel);
  addAndMakeVisible(sPos);
  addAndMakeVisible(sPan);
  addAndMakeVisible(sCoarse);
  addAndMakeVisible(sFine);
  addAndMakeVisible(&graph);
}

OscillatorEditor::~OscillatorEditor() { setLookAndFeel(nullptr); }

void OscillatorEditor::resized()
{
  // aspect ratio is fixed at 2:3
  auto lBounds = getLocalBounds().toFloat();
  const float dX = lBounds.getWidth() / 14.0f;
  auto graphArea = lBounds.removeFromTop(dX * 11.0f);
  auto coarseArea = graphArea.removeFromRight(dX * 5.0f);
  auto fineArea = coarseArea.removeFromTop(dX * 5.0f);
  auto labelArea = graphArea.removeFromTop(dX * 2.0f);
  auto lowerKnobWidth = lBounds.getWidth() / 3.0f;
  auto panArea = lBounds.removeFromTop(lowerKnobWidth);
  auto posArea = panArea.removeFromLeft(lowerKnobWidth);
  auto levelArea = panArea.removeFromLeft(lowerKnobWidth);

  sCoarse.setBounds(coarseArea.toNearestInt());
  sFine.setBounds(fineArea.toNearestInt());
  graph.setBounds(graphArea.toNearestInt());
  sPos.setBounds(posArea.toNearestInt());
  sLevel.setBounds(levelArea.toNearestInt());
  sPan.setBounds(panArea.toNearestInt());
}

void OscillatorEditor::paint(Graphics &g)
{
  const String labelText = "Oscillator " + String(index + 1);
  float dX = (float)getWidth() / 14.0f;
  Rectangle<float> labelBounds(0.0f, 0.0f, 9.0f * dX, 2.0f * dX);
  auto bgColor = getLookAndFeel().findColour(Label::ColourIds::backgroundColourId);
  g.fillAll(bgColor);
  auto textColor = getLookAndFeel().findColour(Label::ColourIds::textColourId);
  g.setColour(textColor);
  g.drawFittedText(labelText, labelBounds.toNearestInt(), Justification::centred, 1);
}
