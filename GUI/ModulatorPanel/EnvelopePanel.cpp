#include "EnvelopePanel.h"

EnvelopeParamLabel::EnvelopeParamLabel(EVT* tree, const String& id) :
state(tree),
paramID(id)
{
    addAndMakeVisible(&label);
    label.setEditable(false, true);
    label.addListener(this);
    attach.reset(new ParameterAttachment(*state->getAPVTS()->getParameter(paramID), [this](float f){ setValue(f); }));
    attach->sendInitialUpdate();
}

EnvelopeParamLabel::~EnvelopeParamLabel()
{
    label.removeListener(this);
}

void EnvelopeParamLabel::labelTextChanged(Label* l) 
{
    float value = std::stof(l->getText().toStdString());
    auto pRange = state->getAPVTS()->getParameterRange(paramID);
    value = Math::fconstrain(pRange.start, pRange.end, value);
    attach->setValueAsCompleteGesture(value);
}

void EnvelopeParamLabel::setValue(float val)
{
    String valStr(val);
    valStr = valStr.substring(0, 6);
    if(valStr.length() < 6 && valStr.contains("."))
    {
        while(valStr.length() < 6)
        {
            valStr += '0';
        }
    }
    else if(valStr.length() > 0)
    {
        valStr += ".";
        while(valStr.length() < 6)
        {
            valStr += '0';
        }
    }
    else
    {
        valStr = "0.0000";
    }
    label.setText(valStr, dontSendNotification);
}
//========================================================================
EnvelopePanel::EnvelopePanel(EVT* tree, int idx) : 
state(tree), 
index(idx),
graph(tree, idx),
attackLabel(tree, IDs::attackMs.toString() + String(idx)),
holdLabel(tree, IDs::holdMs.toString() + String(idx)),
decayLabel(tree, IDs::decayMs.toString() + String(idx)),
sustainLabel(tree, IDs::sustainLevel.toString() + String(idx)),
releaseLabel(tree, IDs::releaseMs.toString() + String(idx))
{
    addAndMakeVisible(graph);
    for(auto l : labels)
    {
        addAndMakeVisible(l);
    }
}

void EnvelopePanel::resized()
{
    auto lBounds = getLocalBounds().toFloat();
    const float labelHeight = lBounds.getHeight() / 8.0f;
    auto labelArea = lBounds.removeFromBottom(labelHeight);
    auto lWidth = labelArea.getWidth() / (float)labels.size();
    for(int i = 0; i < labels.size(); i++)
    {
        labels[i]->setBounds(labelArea.removeFromLeft(lWidth).toNearestInt());
    }
    graph.setBounds(lBounds.toNearestInt());
}