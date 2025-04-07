#pragma once
#include "Electrum/GUI/Util/ClickableComponent.h"
#include "Electrum/Identifiers.h"
#include "Electrum/Shared/FileSystem.h"
#include "GUITypedefs.h"
#include "../Shared/ElectrumState.h"
#include "juce_graphics/juce_graphics.h"

// View for saving the current patch
class PatchSaver : public Component,
                   public juce::TextEditor::Listener,
                   public juce::ComboBox::Listener {
private:
  ElectrumState* state;
  patch_meta_t patch;
  BoundedAttString topLabel;
  BoundedAttString nameLabel;
  juce::TextEditor nameBox;
  BoundedAttString categLabel;
  juce::ComboBox categBox;
  BoundedAttString authorLabel;
  juce::TextEditor authorBox;
  BoundedAttString descLabel;
  juce::TextEditor descBox;

  juce::TextButton cancelBtn;
  juce::TextButton saveBtn;

  std::array<BoundedAttString*, 5> attStrings = {
      &topLabel, &nameLabel, &categLabel, &authorLabel, &descLabel};

public:
  PatchSaver(ElectrumState* s);
  void textEditorTextChanged(juce::TextEditor& te) override;
  void comboBoxChanged(juce::ComboBox* cb) override;
  void resized() override;
  void paint(juce::Graphics& g) override;
};

//===================================================================
// component to represent a single patch to be displayed in
// a list
class PatchListEntry : public ClickableComponent {
private:
  patch_meta_t* const patch;
  bool isSelected = false;
  AttString nameText;
  AttString authorText;

public:
  PatchListEntry(patch_meta_t* p);
  patch_meta_t* getPatch() { return patch; }
  void paint(juce::Graphics& g) override;
  void setSelected(bool sel) {
    isSelected = sel;
    repaint();
  }
  static int compareElements(PatchListEntry* first, PatchListEntry* second);
};

//===================================================================
// drop-down menu button
class DropDownBtn : public juce::Button {
public:
  DropDownBtn() : juce::Button("dropdown") { setToggleable(true); }
  void paintButton(juce::Graphics& g, bool highlighted, bool down) override;
};

class PatchCategHeader : public Component {
private:
  DropDownBtn btn;
  AttString nameText;
  frect_t maxTextBounds;

public:
  PatchCategHeader(int category);
  bool isOpen() const { return btn.getToggleState(); }
  void resized() override;
  void paint(juce::Graphics& g) override;
};

//===================================================================
typedef juce::OwnedArray<PatchListEntry> patch_list_t;

class PatchList : public Component, public ElectrumUserLib::Listener {
private:
  ElectrumState* const state;
  juce::OwnedArray<PatchCategHeader> categHeaders;
  patch_list_t patches;
  void setSelectedPatch(patch_meta_t* p);
  patch_meta_t* selectedPatch = nullptr;

public:
  PatchList(ElectrumState* s);
  ~PatchList() override;
  patch_meta_t* getSelected() { return selectedPatch; }
  void patchWasSaved(patch_meta_t* patch) override;
  void resized() override;

  // private:
  // void sortPatchList();
};

class PatchViewport : public Component {
private:
  PatchList pl;
  juce::Viewport vpt;

public:
  PatchList& getPL() { return pl; }
  PatchViewport(ElectrumState* s) : pl(s) {
    vpt.setViewedComponent(&pl, false);
    vpt.setViewPosition(0, 0);
    vpt.setInterceptsMouseClicks(true, true);
    addAndMakeVisible(vpt);
    pl.resized();
  }
  void resized() override { vpt.setBounds(getLocalBounds()); }
};

//==================================================================
// the top-level panel for all saving and loading business
class PatchBrowser : public Component {
private:
  ElectrumState* const state;
  PatchViewport loader;
  PatchSaver saver;
  juce::TextButton loadBtn;
  juce::TextButton saveBtn;

  void openSaveView();
  void loadCurrentPatch();

public:
  PatchBrowser(ElectrumState* s);
  void resized() override;
};
