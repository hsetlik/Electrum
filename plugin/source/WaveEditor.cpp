#include "Electrum/GUI/WaveEditor/WaveEditor.h"
#include "Electrum/GUI/LookAndFeel/Color.h"
#include "Electrum/GUI/WaveEditor/EditValueTree.h"
#include "Electrum/GUI/WaveEditor/TimeView.h"
#include "Electrum/Identifiers.h"
#include "juce_core/juce_core.h"
#include "juce_events/juce_events.h"

//===================================================

WaveEditor::WaveEditor(ElectrumState* s, Wavetable* wt, int idx)
    : state(s),
      wavetable(wt),
      oscID(idx),
      thumbBar(wt->toString()),
      timeView(nullptr) {
  // 1. figure out which file we need to load
  String pathID = ID::oscWavePath.toString() + String(oscID);
  String path = "Default";
  if (state->state.hasProperty(pathID)) {
    path = state->state.getProperty(pathID);
  } else {
    state->state.setProperty(pathID, path, state->undoManager);
  }
  // 2. parse as a valueTree
  waveTree = WaveEdit::getWavetableTree(path);
  jassert(waveTree.isValid() && waveTree.hasType(WaveEdit::WAVETABLE));

  // 3. add and place the buttons
  closeBtn.setButtonText("Close");
  addAndMakeVisible(&closeBtn);
  saveBtn.setButtonText("Save");
  addAndMakeVisible(saveBtn);
  closeBtn.onClick = [this]() { ModalParent::exitModalView(this); };
  // 4. add the text editor
  addAndMakeVisible(&waveNameEdit);
  String name = waveTree[WaveEdit::waveName];

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
    auto fullStr = WaveEdit::getFullWavetableString(waveTree);
    jassert(state->userLib.attemptWaveSave(waveMeta, fullStr));
    ModalParent::exitModalView(this);
  };
  waveNameEdit.addListener(this);
  waveNameEdit.setText(name, juce::dontSendNotification);
  // 6. add the thumb bar
  addAndMakeVisible(thumbBar);
  // 7. add the time view
  timeView.reset(new TimeView(waveTree));
  addAndMakeVisible(*timeView);
  thumbBar.addListener(timeView.get());
}

WaveEditor::~WaveEditor() {
  auto parent = getParentComponent();
  if (parent != nullptr) {
    parent->removeChildComponent(this);
  }
}

void WaveEditor::resized() {
  auto fBounds = getLocalBounds().toFloat();
  auto bottomRow = fBounds.removeFromBottom(30.0f);
  auto tBounds =
      bottomRow.removeFromLeft(fBounds.getWidth() * 0.66f).reduced(2.5f);
  auto closeBounds =
      bottomRow.removeFromLeft(bottomRow.getWidth() / 2.0f).reduced(2.5f);
  auto saveBounds = bottomRow.reduced(2.5f);

  auto thumbBounds = fBounds.removeFromBottom(60.0f);

  const float viewW = fBounds.getWidth() * 0.95f;
  const float viewH = fBounds.getHeight() * 0.95f;
  auto viewBounds = fBounds.withSizeKeepingCentre(viewW, viewH);
  timeView->setBounds(viewBounds.toNearestInt());

  waveNameEdit.setBounds(tBounds.toNearestInt());
  saveBtn.setBounds(saveBounds.toNearestInt());
  closeBtn.setBounds(closeBounds.toNearestInt());
  thumbBar.setBounds(thumbBounds.toNearestInt());
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
