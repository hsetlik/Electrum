#include "EnvelopePanel.h"

EnvelopePanel::EnvelopePanel(EVT* tree, int idx) : 
state(tree), 
graph(state, idx),
index(idx)
{
    addAndMakeVisible(&graph);
}

void EnvelopePanel::resized() 
{
    auto lBounds = getLocalBounds();
    //auto labelArea = lBounds.removeFromBottom(ENV_LABEL_HEIGHT);
    graph.setBounds(lBounds);

}
