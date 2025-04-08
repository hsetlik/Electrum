#include "Electrum/GUI/PatchBrowser.h"
#include "Electrum/GUI/LookAndFeel/Color.h"
#include "Electrum/GUI/LookAndFeel/Fonts.h"
#include "Electrum/Shared/FileSystem.h"
#include "juce_graphics/juce_graphics.h"

PatchSaver::PatchSaver(ElectrumState* s) : state(s) {
  // 1. set up labels
  auto color = UIColor::defaultText;

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
    getParentComponent()->resized();
  };
  addAndMakeVisible(cancelBtn);
  saveBtn.setButtonText("Save");
  saveBtn.onClick = [this]() {
    state->userLib.attemptPatchSave(state, patch);
    setVisible(false);
    setEnabled(false);
    getParentComponent()->resized();
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
  topLabel.bounds = fBounds.removeFromTop(25.0f);
  nameLabel.bounds = fBounds.removeFromTop(18.0f);
  auto nBounds = fBounds.removeFromTop(18.0f).toNearestInt();
  nameBox.setBounds(nBounds);
  categLabel.bounds = fBounds.removeFromTop(18.0f);
  auto cBounds = fBounds.removeFromTop(18.0f).toNearestInt();
  categBox.setBounds(cBounds);
  authorLabel.bounds = fBounds.removeFromTop(18.0f);
  auto aBounds = fBounds.removeFromTop(18.0f).toNearestInt();
  authorBox.setBounds(aBounds);
  descLabel.bounds = fBounds.removeFromTop(18.0f);
  auto dBounds = fBounds.removeFromTop(18.0f).toNearestInt();
  descBox.setBounds(dBounds);
  auto buttonsHeight = std::min(fBounds.getHeight() / 2.0f, 55.0f);
  auto saveBounds = fBounds.removeFromBottom(buttonsHeight);
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
  auto fill = UIColor::menuBkgnd;
  g.setColour(fill);
  g.fillRect(fBounds);
  // draw all the text
  for (auto* s : attStrings) {
    s->draw(g);
  }
}
//===================================================

PatchListEntry::PatchListEntry(patch_meta_t* p) : patch(p) {
  nameText.setText(patch->name);
  authorText.setText(patch->author);
  authorText.setColour(UIColor::defaultText);
  nameText.setColour(UIColor::defaultText);
  nameText.setFont(FontData::getFontWithHeight(FontE::FuturaLC, 13.0f));
  authorText.setFont(FontData::getFontWithHeight(FontE::FuturaLC, 13.0f));
  nameText.setJustification(juce::Justification::centred);
  authorText.setJustification(juce::Justification::centred);
}

int PatchListEntry::compareElements(PatchListEntry* a, PatchListEntry* b) {
  if (a->getPatch()->category < b->getPatch()->category) {
    return -1;
  } else if (a->getPatch()->category > b->getPatch()->category) {
    return 1;
  } else {  // in the same category just sort alphabetically
    return a->getPatch()->name.compare(b->getPatch()->name);
  }
}

void PatchListEntry::paint(juce::Graphics& g) {
  nameText.setText(patch->name);
  authorText.setText(patch->author);
  // 1. fill the background
  auto fBounds = getLocalBounds().toFloat();
  g.setColour(UIColor::menuBkgnd);
  g.fillRect(fBounds);
  // 2. set the text color
  auto color = isSelected ? UIColor::highlightedText : UIColor::defaultText;
  // 3. draw the outline if this is the selected entry
  if (isSelected) {
    g.setColour(color);
    g.drawRect(fBounds);
  }

  fBounds.removeFromLeft(3.5f);
  // 4. place and draw the text
  // fBounds = fBounds.reduced(1.0f);
  nameText.setColour(color);
  authorText.setColour(color);
  juce::TextLayout nLayout;
  juce::TextLayout aLayout;
  nLayout.createLayout(nameText, fBounds.getWidth() / 2.0f);
  aLayout.createLayout(authorText, fBounds.getWidth() / 2.0f);
  auto nBounds = fBounds.removeFromLeft(nLayout.getWidth());
  nLayout.draw(g, nBounds);
  aLayout.draw(g, fBounds);
}

//===================================================

void DropDownBtn::paintButton(juce::Graphics& g, bool, bool) {
  auto fBounds = getLocalBounds().toFloat();
  juce::Path path;
  path.startNewSubPath(0.0f, 0.0f);
  if (getToggleState()) {  // arrow down
    auto xMid = fBounds.getWidth() / 2.0f;
    path.lineTo(xMid, fBounds.getHeight());
    path.lineTo(fBounds.getWidth(), 0.0f);
  } else {
    auto yMid = fBounds.getHeight() / 2.0f;
    path.lineTo(fBounds.getWidth(), yMid);
    path.lineTo(0.0f, fBounds.getHeight());
  }
  path.closeSubPath();
  g.setColour(UIColor::defaultFill);
  g.fillPath(path);
}

PatchCategHeader::PatchCategHeader(int c) {
  addAndMakeVisible(btn);
  btn.setClickingTogglesState(true);
  setInterceptsMouseClicks(true, true);
  // set up button callback
  btn.onClick = [this]() {
    auto* parent = getParentComponent();
    if (parent != nullptr) {
      parent->resized();
    }
  };
  nameText.setText(UserFiles::PatchCategStrings[c]);
  nameText.setFont(FontData::getFontWithHeight(FontE::RobotoMI, 16.0f));
  nameText.setJustification(juce::Justification::centredLeft);
  nameText.setColour(UIColor::defaultText);
}

void PatchCategHeader::resized() {
  auto fBounds = getLocalBounds().toFloat();
  auto tHeight = fBounds.getHeight() - 3.0f;
  auto tWidth = fBounds.getWidth() - 5.0f;
  fBounds = fBounds.withSizeKeepingCentre(tWidth, tHeight);
  static const float maxBtnSize = 13.0f;
  auto btnBox = fBounds.removeFromRight(tHeight);
  btnBox = btnBox.withSizeKeepingCentre(maxBtnSize, maxBtnSize);
  btn.setBounds(btnBox.toNearestInt());
  maxTextBounds = fBounds;
}

void PatchCategHeader::paint(juce::Graphics& g) {
  // 1. fill the background
  auto fBounds = getLocalBounds().toFloat();
  g.setColour(UIColor::menuBkgnd);
  g.fillRect(fBounds);
  // 2. draw the text
  nameText.draw(g, maxTextBounds);
}

//===================================================
//
// void PatchList::sortPatchList() {
//   patches.sort(PatchListEntry::compareElements);
// }

PatchList::PatchList(ElectrumState* s) : state(s) {
  // 1. add the category headers
  for (int i = 0; i < NUM_PATCH_CATEGORIES; ++i) {
    auto* header = categHeaders.add(new PatchCategHeader(i));
    addAndMakeVisible(header);
  }
  // 2. add the system patches from the UserLib
  const int numPatches = state->userLib.numPatches();
  for (int i = 0; i < numPatches; ++i) {
    auto p = state->userLib.getPatchAtIndex(i);
    auto comp = patches.add(new PatchListEntry(p));
    addAndMakeVisible(comp);
    comp->onClick = [this, p]() { setSelectedPatch(p); };
    comp->onDoubleClick = [this, p]() {
      setSelectedPatch(p);
      auto* pBrowser = findParentComponentOfClass<PatchBrowser>();
      if (pBrowser != nullptr) {
        pBrowser->loadCurrentPatch();
      }
    };
  }
  // sortPatchList();
  //  3. attach the listener
  state->userLib.addListener(this);
}

PatchList::~PatchList() {
  state->userLib.removeListener(this);
}

void PatchList::patchWasSaved(patch_meta_t* p) {
  auto comp = patches.add(new PatchListEntry(p));
  addAndMakeVisible(comp);
  comp->onClick = [this, p]() { setSelectedPatch(p); };
  // sortPatchList();
  resized();
}

void PatchList::setSelectedPatch(patch_meta_t* current) {
  if (selectedPatch != current) {
    selectedPatch = current;
    for (auto* p : patches) {
      if (p->getPatch() == current) {
        p->setSelected(true);
      } else {
        p->setSelected(false);
      }
    }
    resized();
  }
}

void PatchList::resized() {
  static const int hHeader = 25;
  static const int hEntry = 21;
  const int width = std::max(getLocalBounds().getWidth(), 250);
  int currentY = 0;
  for (int i = 0; i < NUM_PATCH_CATEGORIES; ++i) {
    // 1. place the header
    categHeaders[i]->setBounds(0, currentY, width, hHeader);
    currentY += hHeader;
    // 2. place the appropriate entries
    for (auto* p : patches) {
      if (p->getPatch()->category == i) {
        if (categHeaders[i]->isOpen()) {
          p->setVisible(true);
          p->setEnabled(true);
          p->toFront(true);
          p->setBounds(0, currentY, width, hEntry);
          p->repaint();
          currentY += hEntry;
        } else {
          p->setVisible(false);
          p->setEnabled(false);
        }
      }
    }
  }
  setSize(width, currentY);
  setBounds(0, 0, width, currentY);
}

//==================================================================

PatchBrowser::PatchBrowser(ElectrumState* s) : state(s), loader(s), saver(s) {
  loadBtn.setButtonText("Load Patch");
  saveBtn.setButtonText("Save Patch");
  loadBtn.onClick = [this]() { loadCurrentPatch(); };
  saveBtn.onClick = [this]() { openSaveView(); };
  addAndMakeVisible(loadBtn);
  addAndMakeVisible(saveBtn);
  addAndMakeVisible(loader);
  addAndMakeVisible(saver);
  saver.setVisible(false);
  saver.setEnabled(false);
}

void PatchBrowser::resized() {
  if (!saver.isVisible()) {
    saveBtn.setEnabled(true);
    loadBtn.setEnabled(true);
    loader.setEnabled(true);
    loader.setVisible(true);
    loader.toFront(true);
  }
  auto fBounds = getLocalBounds().toFloat();
  auto bBounds = fBounds.removeFromBottom(35.0f);
  auto sBounds = bBounds.removeFromLeft(bBounds.getWidth() / 2.0f);
  loadBtn.setBounds(sBounds.reduced(3.5f).toNearestInt());
  saveBtn.setBounds(bBounds.reduced(3.5f).toNearestInt());
  loader.setBounds(fBounds.toNearestInt());
  saver.setBounds(fBounds.toNearestInt());
}

void PatchBrowser::openSaveView() {
  loader.setVisible(false);
  loader.setEnabled(false);
  saveBtn.setEnabled(false);
  loadBtn.setEnabled(false);
  saver.setEnabled(true);
  saver.setVisible(true);
  saver.toFront(true);
  resized();
}

void PatchBrowser::loadCurrentPatch() {
  if (loader.getPL().getSelected() != nullptr) {
    auto nState =
        state->userLib.getMasterTreeForPatch(loader.getPL().getSelected());
    jassert(nState.isValid());
    state->replaceState(nState);
  }
}
