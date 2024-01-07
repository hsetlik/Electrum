#pragma once
#include "../../Parameters/ElectrumValueTree.h"
#include "../Color.h"
#include "../Fonts.h"

class PatchSelectorPanel : public Component
{
private:
  EVT *const state;

public:
  PatchSelectorPanel(EVT *tree);
  void resized() override;
};
