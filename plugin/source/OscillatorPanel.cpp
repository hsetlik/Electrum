#include "Electrum/GUI/Wavetable/OscillatorPanel.h"
#include "Electrum/Common.h"
#include "Electrum/GUI/LayoutHelpers.h"
#include "Electrum/GUI/Util/ModalParent.h"
#include "Electrum/Identifiers.h"
#include "juce_core/juce_core.h"

OscillatorPanel::OscillatorPanel(ElectrumState* s, int id)
    : state(s),
      sCoarse(s, (id * 5)),
      sFine(s, (id * 5) + 1),
      sPos(s, (id * 5) + 2),
      sLevel(s, (id * 5) + 3),
      sPan(s, (id * 5) + 4),
      graph(s, id),
      oscID(id) {
  addAndMakeVisible(sCoarse);
  addAndMakeVisible(sFine);
  addAndMakeVisible(sPos);
  addAndMakeVisible(sLevel);
  addAndMakeVisible(sPan);
  addAndMakeVisible(graph);
  // now set up the comboBox and Listener
  wavetableCB.addItemList(state->userLib.getAvailableWaveNames(), 1);
  wavetableCB.setSelectedItemIndex(0);
  addAndMakeVisible(wavetableCB);
  wavetableCB.addListener(this);

  addAndMakeVisible(editBtn);
  editBtn.onClick = [this]() {
    ModalParent::openWaveEditor(&editBtn, state, &state->audioData.wOsc[oscID],
                                oscID);
  };
}

void OscillatorPanel::comboBoxChanged(juce::ComboBox* cb) {
  jassert(cb == &wavetableCB);
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
  static const float editBarHeight = 28.0f;
  auto editBounds = remaining.removeFromTop(editBarHeight);
  auto editButtonBounds = editBounds.removeFromRight(editBarHeight);
  wavetableCB.setBounds(editBounds.reduced(3.0f).toNearestInt());
  editBtn.setBounds(editButtonBounds.toNearestInt());
  auto graphBounds = remaining.toNearestInt();
  // juce::ignoreUnused(graphBounds);
  graph.setBounds(graphBounds);
}

void OscillatorPanel::paint(juce::Graphics& g) {
  juce::ignoreUnused(g);
}
//===================================================
