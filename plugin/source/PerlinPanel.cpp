#include "Electrum/GUI/ModulatorPanel/PerlinPanel.h"
#include "Electrum/GUI/LookAndFeel/Color.h"
#include "Electrum/GUI/LookAndFeel/Fonts.h"
#include "Electrum/Identifiers.h"

PerlinGraph::PerlinGraph(ElectrumState* s, int idx) : state(s), perlinID(idx) {
  s->graph.addListener(this);
}

PerlinGraph::~PerlinGraph() {
  state->graph.removeListener(this);
}

void PerlinGraph::graphingDataUpdated(GraphingData* gd) {
  const float _level = gd->getPerlinLevel(perlinID);
  levels[head] = _level;
  head = (head + 1) % PERLIN_GRAPH_POINTS;
  triggerAsyncUpdate();
}

void PerlinGraph::handleAsyncUpdate() {
  repaint();
}

void PerlinGraph::paint(juce::Graphics& g) {
  auto fBounds = getLocalBounds().toFloat();
  g.setColour(UIColor::windowBkgnd);
  g.fillRect(fBounds);
  g.setColour(Color::qualifierPurple);
  g.drawRect(fBounds, 1.8f);
  juce::Path path;
  const float dX = fBounds.getWidth() / (float)PERLIN_GRAPH_POINTS;
  float x = 0.0f;
  float y = fBounds.getBottom() - (fBounds.getHeight() * levels[head]);
  path.startNewSubPath(x, y);
  for (size_t i = 1; i < PERLIN_GRAPH_POINTS; ++i) {
    auto idx = (head + i) % PERLIN_GRAPH_POINTS;
    x += dX;
    y = fBounds.getBottom() - (fBounds.getHeight() * levels[idx]);
    path.lineTo(x, y);
  }
  juce::PathStrokeType pst(1.8f);
  g.strokePath(path, pst);
}

//===================================================

PerlinComponent::PerlinComponent(ElectrumState* s, int idx) : graph(s, idx) {
  addAndMakeVisible(graph);

  // sliders -----------------------------------------
  freqSlider.setSliderStyle(juce::Slider::Rotary);
  freqSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 1, 1);
  addAndMakeVisible(freqSlider);
  String freqID = ID::perlinFrequency.toString() + String(idx);
  freqAttach.reset(new apvts::SliderAttachment(*s, freqID, freqSlider));

  octSlider.setSliderStyle(juce::Slider::Rotary);
  octSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 1, 1);
  addAndMakeVisible(octSlider);
  String octID = ID::perlinOctaves.toString() + String(idx);
  octAttach.reset(new apvts::SliderAttachment(*s, octID, octSlider));

  lacSlider.setSliderStyle(juce::Slider::Rotary);
  lacSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 1, 1);
  addAndMakeVisible(lacSlider);
  String lacID = ID::perlinLacunarity.toString() + String(idx);
  lacAttach.reset(new apvts::SliderAttachment(*s, lacID, lacSlider));
  // labels ------------------------------------------
  freqLabel.aString.setText("Speed");
  freqLabel.aString.setFont(
      FontData::getFontWithHeight(FontE::RobotoMI, 14.0f));
  freqLabel.aString.setJustification(juce::Justification::centred);
  freqLabel.aString.setColour(UIColor::defaultText);

  octLabel.aString.setText("Octaves");
  octLabel.aString.setFont(FontData::getFontWithHeight(FontE::RobotoMI, 14.0f));
  octLabel.aString.setJustification(juce::Justification::centred);
  octLabel.aString.setColour(UIColor::defaultText);

  lacLabel.aString.setText("Lacunarity");
  lacLabel.aString.setFont(FontData::getFontWithHeight(FontE::RobotoMI, 14.0f));
  lacLabel.aString.setJustification(juce::Justification::centred);
  lacLabel.aString.setColour(UIColor::defaultText);
}

void PerlinComponent::resized() {
  auto fBounds = getLocalBounds().toFloat();
  auto bottomBar = fBounds.removeFromBottom(fBounds.getHeight() / 4.5f);
  auto dY = bottomBar.getWidth() / 3.0f;

  auto freqArea = bottomBar.removeFromLeft(dY);
  freqLabel.bounds = freqArea.removeFromTop(16.0f);
  freqSlider.setBounds(freqArea.toNearestInt());

  auto octArea = bottomBar.removeFromLeft(dY);
  octLabel.bounds = octArea.removeFromTop(16.0f);
  octSlider.setBounds(octArea.toNearestInt());

  auto lacArea = bottomBar;
  lacLabel.bounds = lacArea.removeFromTop(16.0f);
  lacSlider.setBounds(lacArea.toNearestInt());

  auto gBounds = fBounds.reduced(6.0f);
  graph.setBounds(gBounds.toNearestInt());
}

void PerlinComponent::paint(juce::Graphics& g) {
  auto fBounds = getLocalBounds().toFloat();
  // 1. outline/background
  g.setColour(UIColor::widgetBkgnd);
  g.fillRect(fBounds);
  g.setColour(UIColor::defaultText);
  g.drawRect(fBounds, 2.5f);

  freqLabel.draw(g);
  octLabel.draw(g);
  lacLabel.draw(g);
}
