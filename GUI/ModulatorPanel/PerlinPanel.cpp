#include "PerlinPanel.h"
PerlinPanel::PerlinPanel(EVT* tree) : 
state(tree),
sFreq(Slider::Rotary, Slider::NoTextBox),
sLac(Slider::Rotary, Slider::NoTextBox),
sOct(Slider::Rotary, Slider::NoTextBox),
graph(state)
{
    freqAttach.reset(new sAttach(*state->getAPVTS(), IDs::perlinFreq.toString(), sFreq));
    lacAttach.reset(new sAttach(*state->getAPVTS(), IDs::perlinLacunarity.toString(), sLac));
    octAttach.reset(new sAttach(*state->getAPVTS(), IDs::perlinOctaves.toString(), sOct));

    addAndMakeVisible(&sFreq);
    addAndMakeVisible(&sLac);
    addAndMakeVisible(&sOct);
    addAndMakeVisible(&graph);

}
void PerlinPanel::resized() 
{
    auto lBounds = getLocalBounds().toFloat();
    auto paramArea = lBounds.removeFromTop(lBounds.getHeight() / 5.0f);
    auto pWidth = paramArea.getWidth() / 4.0f;
    sFreq.setBounds(paramArea.removeFromLeft(pWidth).toNearestInt());
    sLac.setBounds(paramArea.removeFromLeft(pWidth).toNearestInt());
    sOct.setBounds(paramArea.removeFromLeft(pWidth).toNearestInt());

    graph.setBounds((int)lBounds.getX(), (int)lBounds.getY(), GRAPH_FRAME_RATE * GRAPH_LENGTH_S, GRAPH_HEIGHT);
}
void PerlinPanel::paint(Graphics&) 
{
}