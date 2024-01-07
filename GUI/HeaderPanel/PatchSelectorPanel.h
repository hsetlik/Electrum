#pragma once
#include "../../Parameters/ElectrumValueTree.h"
#include "../Color.h"
#include "../Fonts.h"
#include "../Utility/VectorSymbols.h"

class PatchSelectorPanel : public Component
{
private:
  EVT *const state;

  SymbolButton bOpen;
  SymbolButton bSave;

public:
  PatchSelectorPanel(EVT *tree);
  void resized() override;
};
