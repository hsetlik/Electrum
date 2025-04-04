#include "Electrum/GUI/PatchBrowser.h"
#include "Electrum/GUI/LookAndFeel/Color.h"
#include "Electrum/GUI/LookAndFeel/Fonts.h"
#include "Electrum/Shared/FileSystem.h"

PatchSaver::PatchSaver(ElectrumState* s) : state(s) {
  // 1. set up labels
  auto color = getLookAndFeel().findColour(juce::Label::textColourId);

  topLabel.aString.setText("Save Current Patch");
  topLabel.aString.setColour(color);
  topLabel.aString.setFont(FontData::getFontWithHeight(FontE::FuturaBO, 20.0f));
  topLabel.aString.setJustification(juce::Justification::centred);

  nameLabel.aString.setText("Name:");
  nameLabel.aString.setColour(color);
  nameLabel.aString.setFont(
      FontData::getFontWithHeight(FontE::FuturaLC, 14.0f));
  nameLabel.aString.setJustification(juce::Justification::centredLeft);

  categLabel.aString.setText("Category:");
  categLabel.aString.setColour(color);
  categLabel.aString.setFont(
      FontData::getFontWithHeight(FontE::FuturaLC, 14.0f));
  categLabel.aString.setJustification(juce::Justification::centredLeft);

  authorLabel.aString.setText("Author:");
  authorLabel.aString.setColour(color);
  authorLabel.aString.setFont(
      FontData::getFontWithHeight(FontE::FuturaLC, 14.0f));
  authorLabel.aString.setJustification(juce::Justification::centredLeft);

  descLabel.aString.setText("Description:");
  descLabel.aString.setColour(color);
  descLabel.aString.setFont(
      FontData::getFontWithHeight(FontE::FuturaLC, 14.0f));
  descLabel.aString.setJustification(juce::Justification::centredLeft);

  // 2. set up text editors & listeners
  addAndMakeVisible(nameBox);
  nameBox.addListener(this);
  addAndMakeVisible(authorBox);
  authorBox.addListener(this);
  addAndMakeVisible(descBox);
  descBox.setMultiLine(true);
  descBox.addListener(this);

  // 3. populate and add the ComboBox
  categBox.addItemList(UserFiles::PatchCategStrings, 1);
  categBox.addListener(this);
  addAndMakeVisible(categBox);

  // 4. set up the save/cancel buttons
  cancelBtn.setButtonText("Cancel");
  cancelBtn.onClick = [this]() {
    setVisible(false);
    setEnabled(false);
    getParentComponent()->repaint();
  };
  addAndMakeVisible(cancelBtn);
  saveBtn.setButtonText("Save");
  saveBtn.onClick = [this]() {
    state->userLib.attemptPatchSave(state, patch);
    setVisible(false);
    setEnabled(false);
    getParentComponent()->repaint();
  };
  addAndMakeVisible(saveBtn);
  saveBtn.setEnabled(false);
  categBox.setSelectedItemIndex(0);
}

void PatchSaver::textEditorTextChanged(juce::TextEditor& editor) {
  auto* p = &editor;
  if (p == &nameBox) {
    patch.name = editor.getText();
  } else if (p == &authorBox) {
    patch.author = editor.getText();
  } else {
    patch.description = editor.getText();
  }
  saveBtn.setEnabled(state->userLib.validatePatchData(&patch));
}

void PatchSaver::comboBoxChanged(juce::ComboBox* cb) {
  patch.category = cb->getSelectedItemIndex();
  saveBtn.setEnabled(state->userLib.validatePatchData(&patch));
}

void PatchSaver::resized() {
  auto fBounds = getLocalBounds().toFloat();
  fBounds = fBounds.reduced(3.0f);
  topLabel.bounds = fBounds.removeFromTop(21.0f);
  nameLabel.bounds = fBounds.removeFromTop(15.0f);
  auto nBounds = fBounds.removeFromTop(15.0f).toNearestInt();
  nameBox.setBounds(nBounds);
  categLabel.bounds = fBounds.removeFromTop(15.0f);
  auto cBounds = fBounds.removeFromTop(15.0f).toNearestInt();
  categBox.setBounds(cBounds);
  authorLabel.bounds = fBounds.removeFromTop(15.0f);
  auto aBounds = fBounds.removeFromTop(15.0f).toNearestInt();
  authorBox.setBounds(aBounds);
  descLabel.bounds = fBounds.removeFromTop(15.0f);
  auto dBounds = fBounds.removeFromTop(31.0f).toNearestInt();
  descBox.setBounds(dBounds);
  auto saveBounds = fBounds.removeFromBottom(fBounds.getHeight() / 2.0f);
  auto cancelBounds = saveBounds.removeFromLeft(saveBounds.getWidth() / 2.0f);
  saveBtn.setBounds(saveBounds.reduced(2.0f).toNearestInt());
  cancelBtn.setBounds(cancelBounds.reduced(2.0f).toNearestInt());
}

void PatchSaver::paint(juce::Graphics& g) {
  auto border = Color::nearBlack.brighter(0.4f);
  auto fBounds = getLocalBounds().toFloat();
  g.setColour(border);
  g.fillRect(fBounds);
  fBounds = fBounds.reduced(3.0f);
  auto fill = Color::commentGray;
  g.setColour(fill);
  g.fillRect(fBounds);
}
//===================================================
