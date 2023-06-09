#include "EnvelopeTabbedComponent.h"

EnvelopeSourceComponent::EnvelopeSourceComponent(EVT* tree) : state(tree)
{
    for(int i = 0; i < NUM_ENVELOPES; i++)
    {
        String id = IDs::envSource.toString() + String(i);
        sources.add(new ModulationSourceComponent(state, id));
        addAndMakeVisible(sources.getLast());
    }
}

void EnvelopeSourceComponent::resized()
{
    auto lBounds = getLocalBounds().toFloat();
    auto dY = lBounds.getWidth();
    for(auto s : sources)
    {
        auto bounds = lBounds.removeFromTop(dY);
        s->setBounds(bounds.toNearestInt());
    }
}
//===========================================================================

EnvelopeTabbedComponent::EnvelopeTabbedComponent(EVT* tree) : 
selectedEnvIndex(0),
state(tree),
sources(tree)
{
    addAndMakeVisible(sources);
    for(int i = 0; i < NUM_ENVELOPES; i++)
    {
        // add the envelope panels
        envs.add(new EnvelopePanel(state, i));
        addAndMakeVisible(envs.getLast());
        // add the buttons and their callbacks
        buttons.add(new TextButton("Env " + String(i + 1)));
        auto button = buttons.getLast();
        addAndMakeVisible(button);
        button->onClick = [this, i]
        {
            this->setSelectedIndex(i);
        };
    }
}

void EnvelopeTabbedComponent::resized()
{
    auto lBounds = getLocalBounds().toFloat();
    auto srcArea = lBounds.removeFromRight(lBounds.getHeight() / (float)NUM_ENVELOPES);
    sources.setBounds(srcArea.toNearestInt());
    auto buttonsArea = lBounds.removeFromBottom(lBounds.getHeight() / 9.0f);
    int buttonWidth = (int)(lBounds.getWidth() / (float)NUM_ENVELOPES);
    for(int i  = 0; i < NUM_ENVELOPES; i++)
    {
        buttons[i]->setBounds(buttonWidth * i, (int)buttonsArea.getY(), buttonWidth, (int)buttonsArea.getHeight());
        envs[i]->setBounds(lBounds.toNearestInt());
        if (i == selectedEnvIndex)
        {
            buttons[i]->setEnabled(false);
            envs[i]->setVisible(true);
        }
        else
        {
            buttons[i]->setEnabled(true);
            envs[i]->setVisible(false);
        }
    }

}
