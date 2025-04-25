#pragma once
#include "Electrum/GUI/Modulation/ModSourceButton.h"
#include "Electrum/Shared/ElectrumState.h"
#include "EnvelopeGraph.h"
#include "../Modulation/ModSourceComponent.h"
#include "juce_audio_processors/juce_audio_processors.h"

// a label that for displaying & editing the values of an env. parameter
enum env_label_t { attack, hold, decay, sustain, release, curve };

class EnvParamLabel : public juce::Component, juce::Label::Listener {
private:
  ElectrumState* const state;
  const String paramID;
  std::unique_ptr<juce::ParameterAttachment> pAttach;

  juce::Label label;

  float currentParamVal = 0.0f;

public:
  EnvParamLabel(ElectrumState* s, const String& id);
  void labelTextChanged(juce::Label* l) override;
  void resized() override;

private:
  void setTextForValue(float val);
};

//================================================================

// the component for a single envelope
class EnvelopeComponent : public juce::Component {
public:
  const int envID;
  EnvelopeComponent(ElectrumState* s, int id);
  void resized() override;
  void paint(juce::Graphics& g) override;
  void refreshGraph() {
    graph.syncWithState();
    repaint();
  }

private:
  juce::OwnedArray<EnvParamLabel> labels;
  EnvelopeGraph graph;
};

//================================================================

// tabbed component for all the envelopes

class EnvGroupComponent : public Component {
private:
  ElectrumState* const state;
  juce::OwnedArray<EnvelopeComponent> envs;
  juce::OwnedArray<ModSourceButton> buttons;
  //---------------------------------------------------
  int selectedEnv = -1;
  void setSelectedEnv(int idx);

public:
  EnvGroupComponent(ElectrumState* s);
  void resized() override;
};
