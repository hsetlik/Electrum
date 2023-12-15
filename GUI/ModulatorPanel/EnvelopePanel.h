#pragma once
#include "../../Parameters/ElectrumValueTree.h"
#include "../Modulation/SourceParamSlider.h"
#include "EnvelopeGraph.h"
#define ENV_LABEL_HEIGHT 15
#define LEVEL_LABEL_WIDTH 20
struct EnvelopeLabel : public Component, public Label::Listener, public APVTS::Listener
{
private:
  Label label;
  // this doo helps convert the parameter value to real text
  static String labelTextForParam(float value, const String &param);

public:
  EVT *const state;
  const String paramID;
  EnvelopeLabel(EVT *tree, const String &param, AsyncUpdater *ud);
  ~EnvelopeLabel() override;
  void resized() override;
  void labelTextChanged(Label *l) override;
  void parameterChanged(const String &id, float value) override;

private:
  AsyncUpdater *const updater;
};

//===================================================================
class EnvelopePanel : public Component
{
private:
  EVT *const state;
  EnvelopeGraph graph;
  std::unique_ptr<EnvelopeLabel> aLabel;
  std::unique_ptr<EnvelopeLabel> hLabel;
  std::unique_ptr<EnvelopeLabel> dLabel;
  std::unique_ptr<EnvelopeLabel> sLabel;
  std::unique_ptr<EnvelopeLabel> rLabel;
  SourceParamSlider trackingSlider;

public:
  const int index;
  EnvelopePanel(EVT *tree, int idx);
  void resized() override;
};
