#include "EnvelopePanel.h"

EnvelopeLabel::EnvelopeLabel(EVT* tree, const String& param, AsyncUpdater* ud) : 
state(tree), 
paramID(param),
updater(ud)
{
  state->getAPVTS()->addParameterListener(param, this);
  label.addListener(this);
  label.setEditable(true);
  auto value = state->getFloatParamValue(paramID);
  label.setText(labelTextForParam(value, paramID), juce::dontSendNotification);
  addAndMakeVisible(&label);
}

EnvelopeLabel::~EnvelopeLabel()
{
  state->getAPVTS()->removeParameterListener(paramID, this);
  label.removeListener(this);
}

void EnvelopeLabel::resized()
{
  label.setBounds(getLocalBounds());
}

void EnvelopeLabel::parameterChanged(const String& id, float value) 
{
  if(id == paramID)
  {
    label.setText(labelTextForParam(value, id), juce::dontSendNotification);
  }
}

String EnvelopeLabel::labelTextForParam(float value, const String& param)
{
//  DLog::log("Computing string for value: " + String(value));
  if(param.contains(IDs::attackMs.toString()))
  {
    String str(value);
    if(str.length() < 2)
      str += ".0";
    else if(str.length() > 6)
        str = str.substring(0, 6);
    return str;
  }
  else if(param.contains(IDs::holdMs.toString()))
  {
    String str(value);
    if(str.length() < 2)
      str += ".0";
    else if(str.length() > 6)
        str = str.substring(0, 6);
    return str;
  }
  else if(param.contains(IDs::decayMs.toString()))
  {
    String str(value);
    if(str.length() < 2)
      str += ".0";
    else if(str.length() > 6)
        str = str.substring(0, 6);
    return str;
  }
  else if(param.contains(IDs::sustainLevel.toString()))
  {
    if(value == 1.0f)
      return "1.0";
    else if(value == 0.0f)
      return "0.0";
    String fullStr(value);
    return fullStr.substring(0, 4);
  }
  else // releaseMs
  {
    auto iMs = (int)std::floor(value);
    return String(iMs);
  }
}

void EnvelopeLabel::labelTextChanged(Label* l) 
{
  if(l == &label)
  {
    auto str = label.getText(false);
    auto paramRange = state->getAPVTS()->getParameterRange(paramID);
    float value = std::stof(str.toStdString());
    if(!paramRange.getRange().contains(value))
    {
      DLog::log("Out of range value " + str + " for parameter " + paramID);
    }
    else
    {
      auto param = state->getFloatParamPtr(paramID);
      if(param != nullptr)
      {
        *param = value;
        updater->triggerAsyncUpdate();
      }
    }
  }
}

//==================================================================
EnvelopePanel::EnvelopePanel(EVT* tree, int idx) : 
state(tree), 
graph(state, idx),
index(idx)
{
  addAndMakeVisible(&graph);
  String iStr(index);
  String aID = IDs::attackMs.toString() + iStr;
  String hID = IDs::holdMs.toString() + iStr;
  String dID = IDs::decayMs.toString() + iStr;
  String sID = IDs::sustainLevel.toString() + iStr;
  String rID = IDs::releaseMs.toString() + iStr;

  aLabel.reset(new EnvelopeLabel(tree, aID, graph.getCore()));
  hLabel.reset(new EnvelopeLabel(tree, hID, graph.getCore()));
  dLabel.reset(new EnvelopeLabel(tree, dID, graph.getCore()));
  sLabel.reset(new EnvelopeLabel(tree, sID, graph.getCore()));
  rLabel.reset(new EnvelopeLabel(tree, rID, graph.getCore()));

  addAndMakeVisible(aLabel.get());
  addAndMakeVisible(hLabel.get());
  addAndMakeVisible(dLabel.get());
  addAndMakeVisible(sLabel.get());
  addAndMakeVisible(rLabel.get());
}

void EnvelopePanel::resized() 
{
    auto lBounds = getLocalBounds();
    auto labelArea = lBounds.removeFromBottom(ENV_LABEL_HEIGHT);
    auto labelWidth = labelArea.getWidth() / 5;
    
    // auto levelArea = lBounds.removeFromRight(LEVEL_LABEL_WIDTH);
    // level.setBounds(levelArea);
    aLabel->setBounds(labelArea.removeFromLeft(labelWidth));
    hLabel->setBounds(labelArea.removeFromLeft(labelWidth));
    dLabel->setBounds(labelArea.removeFromLeft(labelWidth));
    sLabel->setBounds(labelArea.removeFromLeft(labelWidth));
    rLabel->setBounds(labelArea);
    graph.setBounds(lBounds);
}


