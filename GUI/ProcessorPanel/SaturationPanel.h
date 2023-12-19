#pragma once
#include "../Modulation/LabeledMDS.h"

class SaturationPanel : public Component
{
public:
  EVT *const state;
  SaturationPanel(EVT *tree);
  ~SaturationPanel() override;
  void resized() override;
  void paint(Graphics &g) override;

private:
  LabeledMDS sCoeff;
  LabeledMDS sDrive;
  LabeledMDS sMix;
  ComboBox bSatType;
  std::unique_ptr<APVTS::ComboBoxAttachment> typeAttach;
};
