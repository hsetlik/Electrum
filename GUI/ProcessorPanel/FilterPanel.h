#pragma once
#include "../../Parameters/ElectrumValueTree.h"
#include "../Color.h"
#include "../Modulation/LabeledMDS.h"

class FilterPanel : public Component
{
public:
  EVT *const state;
  FilterPanel(EVT *tree);
  void resized() override;

private:
  LabeledMDS sCutoff;
  LabeledMDS sRes;
  LabeledMDS sMix;
  LabeledMDS sTracking;
  ComboBox bFilterType;
  std::unique_ptr<APVTS::ComboBoxAttachment> typeAttach;
};
