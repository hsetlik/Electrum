#include "Electrum/GUI/WaveEditor/WaveEditor.h"
#include "Electrum/GUI/LookAndFeel/Color.h"
#include "Electrum/GUI/WaveEditor/EditValueTree.h"
#include "Electrum/GUI/WaveEditor/FFTView.h"
#include "Electrum/GUI/WaveEditor/TimeView.h"
#include "Electrum/GUI/WaveEditor/WaveThumbnail.h"
#include "Electrum/Identifiers.h"
#include "juce_core/juce_core.h"
#include "juce_events/juce_events.h"
#include "juce_gui_basics/juce_gui_basics.h"

WaveViewerTabs::WaveViewerTabs(ValueTree& vt)
    : juce::TabbedComponent(TabPositionE::TabsAtBottom) {
  auto col = UIColor::widgetBkgnd;
  addTab("Time", col, new TimeView(vt), true);
  addTab("FFT", col, new FrameSpectrumViewer(vt), true);
}

//===================================================

WaveEditor::WaveEditor(ElectrumState* s, Wavetable* wt, int idx)
    : state(s), oscID(idx), thumbBar(nullptr), tabs(nullptr) {
  juce::ignoreUnused(wt);
  // 1. figure out which file we need to load
  String indexID = ID::oscillatorWaveIndex.toString() + String(oscID);
  const int waveIdx = (int)state->getRawParameterValue(indexID)->load();
  auto waveNames = state->userLib.getAvailableWaveNames();
  String name = waveNames[waveIdx];

  // 2. parse as a valueTree
  waveTree = WaveEdit::getWavetableTree(name);
  jassert(waveTree.isValid() && waveTree.hasType(WaveEdit::WAVETABLE));
  // waveTree.addListener(this);

  // 3. add and place the buttons
  closeBtn.setButtonText("Close");
  addAndMakeVisible(&closeBtn);
  saveBtn.setButtonText("Save");
  addAndMakeVisible(saveBtn);
  closeBtn.onClick = [this]() { ModalParent::exitModalView(this); };
  // 4. add the text editor
  addAndMakeVisible(&waveNameEdit);

  // and grip the existing metadata
  auto* metadata = state->userLib.getWavetableData(name);
  if (metadata != nullptr) {
    waveMeta.name = metadata->name;
    waveMeta.author = metadata->author;
    waveMeta.category = metadata->category;
  }
  // 5. button and listener attachments
  saveBtn.setEnabled(state->userLib.validateWaveData(&waveMeta));
  saveBtn.onClick = [this]() {
    WaveEdit::saveEditsInWaveTree(waveTree);
    auto fullStr = WaveEdit::getFullWavetableString(waveTree);
    jassert(state->userLib.attemptWaveSave(waveMeta, fullStr));
    ModalParent::exitModalView(this);
  };
  waveNameEdit.addListener(this);
  waveNameEdit.setText(name, juce::dontSendNotification);
  // 6. add the thumb bar
  thumbBar.reset(
      new WaveThumbnailBar(WaveEdit::getFullWavetableString(waveTree)));
  addAndMakeVisible(thumbBar.get());
  // 7. add the tabbed view
  tabs.reset(new WaveViewerTabs(waveTree));
  addAndMakeVisible(*tabs);
  thumbBar->addListener(this);
  frameWasFocused(0);
}

WaveEditor::~WaveEditor() {
  auto parent = getParentComponent();
  if (parent != nullptr) {
    parent->removeChildComponent(this);
  }
}

void WaveEditor::frameWasFocused(int frame) {
  p_setFocusedIndex(frame);
}

void WaveEditor::previewEditsOnOscillator() {
  // 1. ask for listeners to update WaveTree
  for (auto* w : watchers) {
    w->waveTreeUpdateRequested();
  }
  //  2. add up the wave string for all the frames
  String str = "";
  for (auto it = waveTree.begin(); it != waveTree.end(); ++it) {
    auto frame = *it;
    jassert(frame.hasType(WaveEdit::WAVE_FRAME));
    auto warp = frame.getChildWithName(WaveEdit::FFT_WARP);
    String frameStr;
    if (warp.isValid()) {
      frameStr = warp[WaveEdit::warpedWaveStringData];
      jassert(frameStr != "null");
    } else {
      frameStr = frame[WaveEdit::frameStringData];
    }
    str += frameStr;
  }
  // 3. re-load the oscillator with the new string
  state->audioData.wOsc[oscID].loadWaveData(str);
}

void WaveEditor::resized() {
  auto fBounds = getLocalBounds().toFloat();
  auto bottomRow = fBounds.removeFromBottom(30.0f);
  auto tBounds =
      bottomRow.removeFromLeft(fBounds.getWidth() * 0.66f).reduced(2.5f);
  auto closeBounds =
      bottomRow.removeFromLeft(bottomRow.getWidth() / 2.0f).reduced(2.5f);
  auto saveBounds = bottomRow.reduced(2.5f);

  auto thumbBounds = fBounds.removeFromBottom(65.0f);

  const float viewW = fBounds.getWidth() * 0.95f;
  const float viewH = fBounds.getHeight() * 0.95f;
  auto viewBounds = fBounds.withSizeKeepingCentre(viewW, viewH);
  tabs->setBounds(viewBounds.toNearestInt());

  waveNameEdit.setBounds(tBounds.toNearestInt());
  saveBtn.setBounds(saveBounds.toNearestInt());
  closeBtn.setBounds(closeBounds.toNearestInt());
  thumbBar->setBounds(thumbBounds.toNearestInt());
}

void WaveEditor::textEditorTextChanged(juce::TextEditor& te) {
  waveMeta.name = te.getText();
  saveBtn.setEnabled(state->userLib.validateWaveData(&waveMeta));
}

void WaveEditor::paint(juce::Graphics& g) {
  auto fBounds = getLocalBounds().toFloat();
  g.setColour(UIColor::widgetBkgnd);
  g.fillRect(fBounds);
}
