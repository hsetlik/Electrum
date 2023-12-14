#pragma once
#include "../../Parameters/ElectrumValueTree.h"
// for now this just has a slider, maybe add a label later?
class SourceParamSlider : public Component
{
private:
  EVT *const state;
  const String paramID;
  Slider slider;
  sAttachPtr attach;

public:
  SourceParamSlider(EVT *tree, const String &ID, bool vertical = false);
  ~SourceParamSlider() override {}
  void resized() override;
};
