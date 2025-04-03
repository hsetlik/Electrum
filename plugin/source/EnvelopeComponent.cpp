#include "Electrum/GUI/ModulatorPanel/EnvelopeComponent.h"
#include "Electrum/GUI/GUITypedefs.h"
#include "Electrum/GUI/LookAndFeel/Color.h"
#include "Electrum/Identifiers.h"
#include "juce_core/juce_core.h"
#include "juce_events/juce_events.h"

static env_label_t labelTypeFor(const String& paramID) {
  if (paramID.contains("Curve"))
    return env_label_t::curve;
  if (paramID.contains("attack"))
    return env_label_t::attack;
  if (paramID.contains("hold"))
    return env_label_t::hold;
  if (paramID.contains("decay"))
    return env_label_t::decay;
  if (paramID.contains("sustain"))
    return env_label_t::sustain;
  return env_label_t::release;
}

static String stringForMs(float value) {
  if (value > 55.0f) {
    int iVal = (int)std::floor(value);
    return String(iVal);
  }
  String vStr(value);
  return vStr.substring(0, 4);
}

static String stringForCurve(float value) {
  value *= 100.0f;
  if (value > 0.13f) {
    int iVal = (int)std::floor(value);
    return String(iVal);
  }
  String vStr(value);
  return vStr.substring(0, 4);
}

static String stringForEnvVal(const String& paramID, float value) {
  env_label_t type = labelTypeFor(paramID);
  switch (type) {
    case attack:
      return "A: " + stringForMs(value);
    case hold:
      return "H: " + stringForMs(value);
    case decay:
      return "D: " + stringForMs(value);
    case sustain:
      return "S: " + stringForCurve(value);
    case release:
      return "R: " + stringForMs(value);
    case curve:
      return stringForMs(value);
  }
  jassert(false);
  return stringForMs(value);
}

//===================================================

EnvParamLabel::EnvParamLabel(ElectrumState* s, const String& id)
    : state(s), paramID(id) {
  label.setJustificationType(juce::Justification::centredLeft);
  label.setEditable(true);
  label.addListener(this);
  setTextForValue(state->getFloatParamValue(paramID));
  auto& param = *state->getParameter(paramID);
  auto callback = [this](float val) { setTextForValue(val); };
  pAttach.reset(
      new juce::ParameterAttachment(param, callback, state->undoManager));
  pAttach->sendInitialUpdate();
  addAndMakeVisible(label);
}

void EnvParamLabel::resized() {
  label.setBounds(getLocalBounds());
}

void EnvParamLabel::labelTextChanged(juce::Label* l) {
  auto str = l->getText();
  str = str.retainCharacters("0123456789.");
  const float fVal = std::stof(str.toStdString());
  auto* param = state->getParameter(paramID);
  param->beginChangeGesture();
  param->setValueNotifyingHost(fVal);
  param->endChangeGesture();
}

void EnvParamLabel::setTextForValue(float val) {
  auto str = stringForEnvVal(paramID, val);
  label.setText(str, juce::dontSendNotification);
}

//===================================================================================

EnvelopeComponent::EnvelopeComponent(ElectrumState* s, int id)
    : state(s), envID(id), graph(s, id) {
  // set up the labels
  String iStr(envID);
  const String aID = ID::attackMs.toString() + iStr;
  labels.add(new EnvParamLabel(s, aID));
  const String hID = ID::holdMs.toString() + iStr;
  labels.add(new EnvParamLabel(s, hID));
  const String dID = ID::decayMs.toString() + iStr;
  labels.add(new EnvParamLabel(s, dID));
  const String sID = ID::sustainLevel.toString() + iStr;
  labels.add(new EnvParamLabel(s, sID));
  const String rID = ID::releaseMs.toString() + iStr;
  labels.add(new EnvParamLabel(s, rID));
  for (int i = 0; i < labels.size(); ++i) {
    addAndMakeVisible(labels[i]);
  }
  addAndMakeVisible(&graph);
}

#define F_BORDER 12.0f
void EnvelopeComponent::resized() {
  auto fBounds = getLocalBounds().toFloat().reduced(F_BORDER);
  // place the labels
  static constexpr float labelHeight = 15.0f;
  auto labelArea = fBounds.removeFromBottom(labelHeight);
  const float dX = labelArea.getWidth() / (float)labels.size();
  for (int i = 0; i < labels.size(); ++i) {
    auto b = labelArea.removeFromLeft(dX);
    labels[i]->setBounds(b.toNearestInt());
  }
  // place the graph
  graph.setBounds(fBounds.toNearestInt());
}

void EnvelopeComponent::paint(juce::Graphics& g) {
  auto fBounds = getLocalBounds().toFloat();
  g.setColour(Color::darkSeaGreen);
  g.fillRect(fBounds);
}

//===================================================================================

EnvGroupComponent::EnvTabs::EnvTabs(EnvGroupComponent* eg)
    : TabBar(TabBar::TabsAtLeft), parent(eg) {
  for (int i = 0; i < NUM_ENVELOPES; ++i) {
    String txt = "Env " + String(i + 1);
    auto bkgnd = Color::nearBlack;
    addTab(txt, bkgnd, i);
  }
}

void EnvGroupComponent::EnvTabs::currentTabChanged(int idx,
                                                   const String& name) {
  if (!parent->isVisible())
    return;
  juce::ignoreUnused(name);
  parent->setSelectedEnv(idx);
}

//-----------------------------------------------------------------------------------

EnvGroupComponent::EnvGroupComponent(ElectrumState* s) : state(s), tabs(this) {
  // create the env components
  for (int i = 0; i < NUM_ENVELOPES; ++i) {
    auto* env = envs.add(new EnvelopeComponent(s, i));
    addAndMakeVisible(env);
  }
  addAndMakeVisible(tabs);
  tabs.setCurrentTabIndex(2, false);
  // setSelectedEnv(0);
}

void EnvGroupComponent::setSelectedEnv(int idx) {
  if (idx != selectedEnv) {
    selectedEnv = idx;
    for (int i = 0; i < NUM_ENVELOPES; ++i) {
      if (i == selectedEnv) {
        envs[i]->setVisible(true);
        envs[i]->setEnabled(true);
      } else {
        envs[i]->setVisible(false);
        envs[i]->setEnabled(false);
      }
    }
  }
  resized();
}

void EnvGroupComponent::resized() {
  auto fBounds = getLocalBounds().toFloat();
  static constexpr float tabWidth = 40.0f;
  auto tabBounds = fBounds.removeFromLeft(tabWidth);
  tabs.setBounds(tabBounds.toNearestInt());
  for (int i = 0; i < envs.size(); ++i) {
    envs[i]->setBounds(fBounds.toNearestInt());
  }
}
