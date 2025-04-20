#include "Electrum/GUI/Wavetable/OscillatorPanel.h"
#include "Electrum/Common.h"
#include "Electrum/GUI/LayoutHelpers.h"
#include "Electrum/GUI/LookAndFeel/BinaryGraphics.h"
#include "Electrum/GUI/Util/ModalParent.h"
#include "Electrum/Identifiers.h"
#include "juce_core/juce_core.h"
#include "juce_events/juce_events.h"

void OscillatorPanel::waveAttachCallback(float fWave) {
  const int waveIdx = (int)fWave;
  jassert(waveIdx < state->userLib.numWavetables());
  wavetableCB.setSelectedItemIndex(waveIdx, juce::dontSendNotification);
}

//======================================================

OscillatorPanel::OscillatorPanel(ElectrumState* s, int id)
    : state(s),
      sCoarse(s, (id * 5)),
      sFine(s, (id * 5) + 1),
      sPos(s, (id * 5) + 2),
      sLevel(s, (id * 5) + 3),
      sPan(s, (id * 5) + 4),
      graph(nullptr),
      oscID(id) {
  graph.reset(new WavetableGraph(s, id));
  addAndMakeVisible(sCoarse);
  addAndMakeVisible(sFine);
  addAndMakeVisible(sPos);
  addAndMakeVisible(sLevel);
  addAndMakeVisible(sPan);
  addAndMakeVisible(graph.get());
  addAndMakeVisible(powerBtn);
  // now set up the comboBox and Listener
  auto waveNames = state->userLib.getAvailableWaveNames();
  wavetableCB.addItemList(waveNames, 1);
  wavetableCB.setSelectedItemIndex(0);
  selectedWaveName = waveNames[0];
  addAndMakeVisible(wavetableCB);
  wavetableCB.addListener(this);
  auto waveLambda = [this](float fVal) { waveAttachCallback(fVal); };
  auto* waveParam =
      state->getParameter(ID::oscillatorWaveIndex.toString() + String(oscID));
  waveAttach.reset(new juce::ParameterAttachment(*waveParam, waveLambda,
                                                 state->undoManager));
  waveAttach->sendInitialUpdate();

  // add this as a userLib listener
  state->userLib.addListener(this);

  addAndMakeVisible(editBtn);
  editBtn.onClick = [this]() {
    ModalParent::openWaveEditor(&editBtn, state, &state->audioData.wOsc[oscID],
                                oscID);
  };
  // set up the button attachment
  String activeID = ID::oscillatorActive.toString() + String(oscID);
  powerAttach.reset(new apvts::ButtonAttachment(*state, activeID, powerBtn));
}

OscillatorPanel::~OscillatorPanel() {
  state->userLib.removeListener(this);
}

void OscillatorPanel::waveWasSaved(wave_meta_t* w) {
  int newID = wavetableCB.getNumItems() + 1;
  wavetableCB.addItem(w->name, newID);
  resized();
}

void OscillatorPanel::comboBoxChanged(juce::ComboBox* cb) {
  String newWaveName = cb->getText();
  if (newWaveName != selectedWaveName) {
    int waveIdx = cb->getSelectedItemIndex();
    waveAttach->setValueAsCompleteGesture((float)waveIdx);
    selectedWaveName = newWaveName;
    state->graph.requestWavetableString(oscID);
    resized();
  }
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
  auto pwrBounds = editBounds.removeFromLeft(editBarHeight);
  powerBtn.setBounds(pwrBounds.reduced(3.0f).toNearestInt());
  auto editButtonBounds = editBounds.removeFromRight(editBarHeight);
  wavetableCB.setBounds(editBounds.reduced(3.0f).toNearestInt());
  editBtn.setBounds(editButtonBounds.toNearestInt());
  auto graphBounds = remaining.toNearestInt();
  graph->setBounds(graphBounds);
}

void OscillatorPanel::paint(juce::Graphics& g) {
  juce::ignoreUnused(g);
}
//===================================================
