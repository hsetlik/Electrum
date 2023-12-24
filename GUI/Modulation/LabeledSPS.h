#pragma once
#include "SourceParamSlider.h"

class LabeledSPS : public Component
{
public:
  LabeledSPS(EVT *tree, const String &paramID, const String &labelText, bool vertical = false);
  ~LabeledSPS() override;
  void resized() override;

private:
  const String text;
  const bool isVertical;
  std::unique_ptr<SourceParamSlider> sps;
  Label label;
};
