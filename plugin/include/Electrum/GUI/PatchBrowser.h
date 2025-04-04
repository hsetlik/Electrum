#pragma once
#include "Electrum/Identifiers.h"
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
//===========================================

