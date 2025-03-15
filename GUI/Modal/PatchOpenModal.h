#pragma once
#include "../../Parameters/ElectrumValueTree.h"
#include "../../Parameters/FileManager.h"
#include "juce_gui_basics/juce_gui_basics.h"

class PatchOpenModal : public Component, public Button::Listener
{
private:
  EVT *const state;
  PatchFileFilter filter;
  FileBrowserComponent browser;
  TextButton okButton;
  TextButton cancelButton;

public:
  PatchOpenModal(EVT *tree);
  void buttonClicked(Button *b) override;
  void resized() override;
};
