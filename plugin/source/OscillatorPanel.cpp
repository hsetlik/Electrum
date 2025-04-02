#include "Electrum/GUI/Wavetable/OscillatorPanel.h"
#include "Electrum/Common.h"
#include "Electrum/GUI/LayoutHelpers.h"
#include "Electrum/Identifiers.h"
#include "juce_core/juce_core.h"

OscillatorPanel::OscillatorPanel(ElectrumState* s, int id)
    : state(s),
      sCoarse(s, (id * 5)),
      sFine(s, (id * 5) + 1),
      sPos(s, (id * 5) + 2),
      sLevel(s, (id * 5) + 3),
      sPan(s, (id * 5) + 4),
      // graph(s, id),
      oscID(id) {
  addAndMakeVisible(sCoarse);
  addAndMakeVisible(sFine);
  addAndMakeVisible(sPos);
  addAndMakeVisible(sLevel);
  addAndMakeVisible(sPan);
  // addAndMakeVisible(graph);
}

void OscillatorPanel::resized() {
  auto fBounds = getLocalBounds().toFloat();
  auto grid = Layout::divideInto(fBounds, 3, 3);
  // place the sliders
  sCoarse.setBounds(grid[2][0].toNearestInt());
  sFine.setBounds(grid[2][1].toNearestInt());
  sLevel.setBounds(grid[2][2].toNearestInt());
  sPan.setBounds(grid[1][2].toNearestInt());
  sPos.setBounds(grid[0][2].toNearestInt());
  frect_t remaining = {0.0f, 0.0f, grid[1][0].getRight(),
                       grid[0][1].getBottom()};
  auto graphBounds = remaining.toNearestInt();
  juce::ignoreUnused(graphBounds);
  // graph.setBounds(graphBounds);
}

void OscillatorPanel::paint(juce::Graphics& g) {
  juce::ignoreUnused(g);
}
//===================================================
