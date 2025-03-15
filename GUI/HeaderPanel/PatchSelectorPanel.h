#pragma once
#include "../../Parameters/ElectrumValueTree.h"
#include "../Color.h"
#include "../Fonts.h"
#include "../Modal/PatchOpenModal.h"
#include "../Utility/VectorSymbols.h"

class PatchSelectorPanel : public Component
{
private:
  EVT *const state;
  Component *const openDialog;
  Component *const saveDialog;

  SymbolButton bOpen;
  SymbolButton bSave;

public:
  PatchSelectorPanel(EVT *tree, Component *loadComp, Component *saveComp);
  ~PatchSelectorPanel() override {}
  void resized() override;
};
