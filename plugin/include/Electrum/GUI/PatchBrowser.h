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
  void paint(juce::Graphics& g) override;
  void setSelected(bool sel) {
    isSelected = sel;
    repaint();
  }
};

//===================================================================
// drop-down menu button
class DropDownBtn : public juce::Button {
public:
  DropDownBtn() : juce::Button("dropdown") { setToggleable(true); }
  void paintButton(juce::Graphics& g, bool highlighted, bool down) override;
};

//===================================================================
class PatchCategHeader : public Component {
private:
  DropDownBtn btn;
  AttString nameText;

public:
  PatchCategHeader(int category);
};

