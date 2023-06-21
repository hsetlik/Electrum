#pragma once
#include "EnvelopeGraph.h"
// editable text boxes for the time values
class EnvelopeParamLabel : 
public Component,
public Label::Listener
{
public:
    EVT* const state;
    const String paramID;
    EnvelopeParamLabel(EVT* tree, const String& id);
    ~EnvelopeParamLabel() override;
    void resized() override
    {
        label.setBounds(getLocalBounds());
    }
    void setValue(float val);
private:
    Label label;
    std::unique_ptr<ParameterAttachment> attach;
    void labelTextChanged(Label* l) override;
};


//=================================================================
class EnvelopePanel : public Component
{
public:
    EVT* const state;
    const int index;
    EnvelopePanel(EVT* tree, int idx);
    void resized() override;
private:
    EnvelopeGraph graph;
    EnvelopeParamLabel attackLabel;
    EnvelopeParamLabel holdLabel;
    EnvelopeParamLabel decayLabel;
    EnvelopeParamLabel sustainLabel;
    EnvelopeParamLabel releaseLabel;
    const std::vector<EnvelopeParamLabel*> labels = { &attackLabel, &holdLabel, &decayLabel, &sustainLabel, &releaseLabel };
};
