#include "EnvelopeTabbedComponent.h"

EnvelopeTabbedComponent::EnvelopeTabbedComponent(EVT* tree) : 
selectedEnvIndex(0),
state(tree)
{
    for(int i = 0; i < NUM_ENVELOPES; i++)
    {
        // add the envelope panels
        envPanels.add(new EnvelopePanel(state, i));
        addAndMakeVisible(envPanels.getLast());
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
    auto buttonsArea = lBounds.removeFromBottom(lBounds.getHeight() / 9.0f);
    int buttonWidth = (int)(lBounds.getWidth() / (float)NUM_ENVELOPES);
    for(int i  = 0; i < NUM_ENVELOPES; i++)
    {
        buttons[i]->setBounds(buttonWidth * i, (int)buttonsArea.getY(), buttonWidth, (int)buttonsArea.getHeight());
        if (i == selectedEnvIndex)
        {
            buttons[i]->setEnabled(false);
            envPanels[i]->setVisible(true);
            envPanels[i]->setBounds(lBounds.toNearestInt());
        }
        else
        {
            buttons[i]->setEnabled(true);
            envPanels[i]->setVisible(false);
        }
    }

}