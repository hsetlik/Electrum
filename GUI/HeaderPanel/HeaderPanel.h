#pragma once
#include "../../Parameters/ElectrumValueTree.h"
#include "../Color.h"
#include "../Fonts.h"
#include "PatchSelectorPanel.h"

#define HEADER_FONT_PX 26.0f

class HeaderPanel : public Component
{
private:
  EVT *const state;
  Font nameFont;
  PatchSelectorPanel patchSelector;

public:
  HeaderPanel(EVT *tree);
  void resized() override;
  void paint(Graphics &g) override;
};
